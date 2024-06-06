#pragma once
#include <functional>
#include "engine/common/chart/line_chart.hpp"
#include "user/common/render/card.hpp"
#include "./gauge.hpp"


struct IterationState
{
    float const background_radius = 20.0f;
    Vec2 const  padding           = Vec2{background_radius, 1.5f * background_radius};
    float const outline           = 5.0f;
    Vec2  const outline_vec       = {outline, outline};
    sf::Color   color             = sf::Color::White;

    Vec2       size;
    Vec2       position;
    Card       background;
    Card       background_outline;
    sf::Font*  font;
    sf::Text   title;
    sf::Text   iteration_idx;
    float      font_scale = 1.0f;


    Gauge gravity_gauge;
    Gauge friction_gauge;

    sf::Text training_time_label;
    sf::Text training_time;

    sf::Text training_time_rt_label;
    sf::Text training_time_rt;

    explicit
    IterationState(Vec2 size_, Vec2 position_ = {})
        : size{size_}
        , position{position_}
        , background{size - 2.0f * outline_vec, background_radius, {50, 50, 50}}
        , background_outline{size, background_radius + outline, color}
        , font{&pez::resources::getFont("font")}
    {
        title.setString("Iteration");
        title.setFont(*font);
        title.setCharacterSize(24);
        title.setFillColor({150, 150, 150});

        iteration_idx.setString("");
        iteration_idx.setFont(*font);
        iteration_idx.setCharacterSize(48);
        iteration_idx.setFillColor(sf::Color::White);

        training_time.setString("");
        training_time.setFont(*font);
        training_time.setCharacterSize(22);
        training_time.setFillColor(sf::Color::White);

        training_time_label.setString("");
        training_time_label.setFont(*font);
        training_time_label.setCharacterSize(16);
        training_time_label.setFillColor({150, 150, 150});

        training_time_label.setString("Simulated training time");

        training_time_rt.setString("");
        training_time_rt.setFont(*font);
        training_time_rt.setCharacterSize(22);
        training_time_rt.setFillColor(sf::Color::White);

        training_time_rt_label.setString("");
        training_time_rt_label.setFont(*font);
        training_time_rt_label.setCharacterSize(16);
        training_time_rt_label.setFillColor({150, 150, 150});

        training_time_rt_label.setString("Real time training time");

        setPosition(position);
    }

    void setSize(Vec2 size_)
    {
        size = size_;
        background = {size - 2.0f * outline_vec, background_radius, {50, 50, 50}};
        background_outline = {size, background_radius + outline, color};
        gravity_gauge.setSize({size.x - 2.0f * padding.x, 20.0f});
        friction_gauge.setSize({size.x - 2.0f * padding.x, 20.0f});
    }

    void setColor(sf::Color c)
    {
        color = c;
        background_outline.setColor(c);
    }

    void setPosition(Vec2 position_)
    {
        position                    = position_;
        background_outline.position = position;
        background.position         = position + outline_vec;

        float const margin = outline + 0.75f * padding.x;

        title.setPosition(position + Vec2{margin, 0.35f * padding.y});

        float current_y = title.getPosition().y + 24.0f;
        iteration_idx.setPosition({position_.x + margin, current_y});

        current_y += 70.0f;
        gravity_gauge.setPosition({position_.x + margin, current_y});

        current_y += 64.0f;
        friction_gauge.setPosition({position_.x + margin, current_y});

        current_y += 64.0f;
        training_time_label.setPosition(position_.x + margin, current_y);

        current_y += 20.0f;
        training_time.setPosition(position_.x + margin, current_y);

        current_y += 40.0f;
        training_time_rt_label.setPosition(position_.x + margin, current_y);

        current_y += 20.0f;
        training_time_rt.setPosition(position_.x + margin, current_y);
    }

    void render(pez::render::Context& context)
    {
        // Background
        background_outline.renderHud(context);
        background.renderHud(context);

        // Render title
        title.setScale(font_scale, font_scale);
        context.drawDirect(title);

        context.drawDirect(iteration_idx);

        gravity_gauge.value.setColor({233, 196, 106});
        gravity_gauge.render(context);

        friction_gauge.value.setColor({231, 111, 81});
        friction_gauge.render(context);

        context.drawDirect(training_time_label);
        context.drawDirect(training_time);

        context.drawDirect(training_time_rt_label);
        context.drawDirect(training_time_rt);
    }

    void setTitle(std::string const& title_)
    {
        title.setString(title_);
    }

    void updateState(float max_friction)
    {
        auto const& state = pez::core::getSingleton<TrainingState>();
        iteration_idx.setString(toString(state.iteration));
        gravity_gauge.setRatio(to<float>(state.configuration.solver_gravity) / 1000.0f);
        gravity_gauge.setTitle("Gravity (" + toString(state.configuration.solver_gravity, 0) + " /1000)");

        friction_gauge.setRatio(to<float>(state.configuration.solver_friction) / max_friction);
        friction_gauge.setTitle("Friction (" + toString(state.configuration.solver_friction, 5) + ")");

        training_time.setString(getTrainingTime(state.iteration, conf::sel::population_size * conf::sel::max_iteration_time));
        training_time_rt.setString(getTrainingTimeRT(state.iteration, 1.2f));
    }

    [[nodiscard]]
    static std::string getTrainingTime(uint32_t iteration, float second_per_iteration)
    {
        auto const time = to<uint32_t>(to<float>(iteration) * second_per_iteration);
        uint32_t hours = time / 3600;
        uint32_t days = hours / 24;
        uint32_t years = days / 365;

        days  = days - years * 365;
        hours = hours - days * 24 - years * 365 * 24;

        std::string const years_txt = (years < 2) ? "year" : "years";
        std::string const days_txt  = (days < 2)  ? "day"  : "days";
        std::string const hours_txt = (hours < 2) ? "hour" : "hours";

        if (years > 0) {
            return toString(years) + " " + years_txt + ", " + toString(days) + " " + days_txt;
        }

        return toString(days) + " " + days_txt + ", " + toString(hours) + " " + hours_txt;
    }

    [[nodiscard]]
    static std::string getTrainingTimeRT(uint32_t iteration, float second_per_iteration)
    {
        auto const time = to<uint32_t>(to<float>(iteration) * second_per_iteration);
        uint32_t minutes = time / 60;
        uint32_t hours = minutes / 60;

        minutes = minutes - hours * 60;
        uint32_t seconds = time - minutes * 60;

        std::string const hours_txt   = (hours < 2)   ? "hour"   : "hours";
        std::string const minutes_txt = (minutes < 2) ? "minute" : "minutes";

        if (hours > 0) {
            return toString(hours) + " " + hours_txt + ", " + toString(minutes) + " " + minutes_txt;
        }

        return toString(minutes) + " " + minutes_txt + ", " + toString(seconds) + " sec";
    }
};