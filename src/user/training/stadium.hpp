#pragma once
#include <filesystem>

#include "engine/engine.hpp"

#include "user/training/training_state.hpp"
#include "user/training/evolver.hpp"
#include "user/training/demo.hpp"
#include "user/training/scene.hpp"
#include "user/training/render/renderer.hpp"


#include "user/common/disturbances.hpp"


struct Stadium : public pez::core::IProcessor
{
    TrainingState&  state;
    tp::ThreadPool& thread_pool;
    Evolver         evolver;

    float target_score = 8.0f;

    bool bypass_score_threshold = false;

    Stadium()
        : state{pez::core::getSingleton<TrainingState>()}
        , thread_pool{pez::core::getSingleton<tp::ThreadPool>()}
    {
        // Create agents info
        pez::core::createMultiple<AgentInfo>(conf::sel::population_size);

        /* Create tasks with training push sequence
           There is one task per agent and each task is forever linked to one agent ID
         */
        for (uint32_t i{0}; i < conf::sel::population_size; ++i) {
            auto task = pez::core::createGetRef<training::Scene>(i, 1);
            task->enable_disturbance = false;
            task->freeze_time = 0.0;
            // Set the task's score function
            task->score_function = [](pbd::RealType pos_x, pbd::RealType out_sum, pbd::RealType dist_sum) {
                pbd::RealType const dist_to_center_penalty = std::abs(1.0 - std::abs(pos_x));
                //return 100.0f / (1.0f + dist_sum + out_sum) * dist_to_center_penalty;
                return 1.0 / (1.0 + out_sum * 0.5) * dist_to_center_penalty;
                //return 100.0f;
            };
        }

        /* Create disturbances
            - 0 is for reference
            - 1 is for training and will be regenerated at each iteration
        */
        pez::core::createMultiple<Disturbances>(2);
        pez::core::get<Disturbances>(0).generateSequence();
        pez::core::get<Disturbances>(1).generateSequence();

        // Create first exploration iteration
        restartExploration();
    }

    /// Loads genome and configuration for the provided generation
    void loadContext(uint32_t gen, bool force_demo = false)
    {
        loadGenome("best_" + toString(gen) + ".bin", force_demo);
        loadConf("best_conf_" + toString(gen) + ".bin");
    }

    /** Loads the genome stored in the specified file
     *
     * @param filename The genome's file
     * @param force_demo If set to true, the demo mode will be enabled to check the loaded genome
     */
    void loadGenome(std::string const& filename, bool force_demo = false)
    {
        // Load the genome once
        nt::Genome genome;
        genome.loadFromFile(filename);

        auto const net = nt::NetworkGenerator().generate(genome);

        // and copy it to all other
        pez::core::foreach<AgentInfo>([&genome](AgentInfo& a) { a.genome = genome; });
        state.demo = force_demo;
    }

    /// Dumps all the genomes of the current generation
    void writeAllGenomes() const
    {
        std::string const path_prefix = getCurrentFolder() + "/dump_" + toString(state.iteration);
        std::filesystem::create_directories(path_prefix);

        for (uint32_t i{0}; i < conf::sel::population_size; ++i) {
            pez::core::get<AgentInfo>(0).genome.writeToFile(path_prefix + "/genome_" + toString(i) + ".bin");
        }
        saveConfiguration(path_prefix + "/configuration.bin");
    }

    /// Load the training configuration stored in the file
    void loadConf(std::string const& filename)
    {
        // Load configuration
        BinaryReader conf_reader{filename};
        conf_reader.readInto(state.configuration.max_speed);
        conf_reader.readInto(state.configuration.max_accel);
        conf_reader.readInto(state.configuration.solver_friction);
        conf_reader.readInto(state.configuration.solver_gravity);
        conf_reader.readInto(state.configuration.solver_sub_steps);
        conf_reader.readInto(state.configuration.solver_compliance);
        conf_reader.readInto(state.configuration.task_sub_steps);

        std::cout << "[Conf loaded]" << std::endl;
        std::cout << "  Gravity: "     << state.configuration.solver_gravity << std::endl;
        std::cout << "  Friction: "    << state.configuration.solver_friction << std::endl;
        std::cout << "  Max speed: "   << state.configuration.max_speed << std::endl;
        std::cout << "  Max accel: "   << state.configuration.max_accel << std::endl;
        std::cout << "  Solver iter: " << state.configuration.solver_sub_steps << std::endl;
        std::cout << "  Solver comp: " << state.configuration.solver_compliance << std::endl;
        std::cout << "  Task iter: "   << state.configuration.task_sub_steps << std::endl;
    }

    /// Performs a training iteration if not in demo mode
    void update(float dt) override
    {
        // Check if we are in the demo, if yes, just skip
        if (state.demo) {
            return;
        }
        // Update state, increases iteration counter and automatically switches to demo mode if needed
        state.addIteration();
        // Run all tasks
        executeTasks(dt);
        // After all tasks has been completed, create the next generation
        evolver.createNewGeneration();
        state.iteration_best_score = pez::core::get<AgentInfo>(0).score;
        // Check if we need to restart exploration
        if (needIncreaseDifficulty()) {
            increaseDifficulty();
        } else if (state.iteration % 10 == 0) {
            saveBest(true);
        }
        auto& renderer = pez::core::getRenderer<training::Renderer>();
        renderer.training_renderer.gravity_plot.addValue(to<float>(state.configuration.solver_gravity));
        renderer.training_renderer.friction_plot.addValue(to<float>(state.configuration.solver_friction));
        renderer.training_renderer.fitness.addValue(to<float>(pez::core::get<AgentInfo>(0).score));
    }

    /// Initializes the iteration
    void initializeIteration() const
    {
        // Only change the training sequence
        pez::core::get<Disturbances>(1).generateSequence();
        // Initialize tasks
        pez::core::parallelForeach<training::Scene>([&](training::Scene& task) {
            // Set the push sequence to the training one
            task.push_sequence_id = 1;
            task.initialize();
        });
    }

    /// Runs all tasks until maximum time is reached
    void executeTasks(float dt)
    {
        initializeIteration();

        uint32_t const tasks_count = pez::core::getCount<training::Scene>();
        auto&          tasks       = pez::core::getData<training::Scene>().getData();
        thread_pool.dispatch(tasks_count, [&](uint32_t start, uint32_t end) {
            float t = 0.0f;
            while (t < conf::sel::max_iteration_time) {
                bool done = true;
                for (uint32_t i{start}; i < end; ++i) {
                    if (!tasks[i].done()) {
                        tasks[i].update(dt);
                        done = false;
                    }
                }
                if (done) {
                    break;
                }
                t += dt;
            }
        });
    }

    /// Saves the genome of the current best agent in a file alongside the current configuration
    void saveBest(bool force = false) const
    {
        if (((state.iteration % conf::exp::best_save_period) == 0) || force) {
            pez::core::get<AgentInfo>(0).genome.writeToFile(getCurrentFolder() + "/best_" + toString(state.iteration) + ".bin");
            // Save configuration
            saveConfiguration(getCurrentFolder() + "/best_conf_" + toString(state.iteration) + ".bin");
        }
    }

    void saveConfiguration(std::string const& filename) const
    {
        BinaryWriter conf_writer{filename};
        conf_writer.write(state.configuration.max_speed);
        conf_writer.write(state.configuration.max_accel);
        conf_writer.write(state.configuration.solver_friction);
        conf_writer.write(state.configuration.solver_gravity);
        conf_writer.write(state.configuration.solver_sub_steps);
        conf_writer.write(state.configuration.solver_compliance);
        conf_writer.write(state.configuration.task_sub_steps);
    }

    [[nodiscard]]
    bool needIncreaseDifficulty() const
    {
        return (state.iteration_best_score > target_score) || bypass_score_threshold;
    }

    void restartExploration()
    {
        state.newExploration();
        // Change the seed of the RNG
        RNGf::setSeed(state.iteration_exploration + conf::exp::seed_offset);
        // Create the folder to save genomes
        std::filesystem::create_directories(getCurrentFolder());
        // Base genome is the last exploration best
        auto const best_genome = pez::core::get<AgentInfo>(0).genome;
        pez::core::foreach<AgentInfo>([&best_genome](AgentInfo& a) {
            a.genome = best_genome;
        });

    }

    [[nodiscard]]
    std::string getCurrentFolder() const
    {
        return "genomes_" + toString(state.iteration_exploration);
    }

    void increaseDifficulty()
    {
        bypass_score_threshold = false;
        // Depending on the configuration, dump the best genome to a file
        saveBest(true);
        if (state.configuration.solver_friction > 0.0f) {
            state.configuration.solver_friction -= 0.000001;
        } else {
            state.configuration.solver_friction = 0.0f;
        }
        if (state.configuration.solver_gravity < conf::sim::max_gravity) {
            state.configuration.solver_gravity *= 1.01f;
            if (state.configuration.solver_gravity > conf::sim::max_gravity) {
                state.configuration.solver_gravity = conf::sim::max_gravity;
            }
        }
        std::cout << "Gravity: " << state.configuration.solver_gravity
                  << " Friction: " << state.configuration.solver_friction
                  << std::endl;
    }
};