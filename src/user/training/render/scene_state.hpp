#pragma once
#include <functional>
#include "engine/common/chart/line_chart.hpp"
#include "user/common/render/card.hpp"
#include "./gauge.hpp"


struct SceneState
{
    float const background_radius = 20.0f;
    float const title_height      = 10.0f;
    Vec2 const  padding           = Vec2{background_radius, 1.5f * background_radius};
    float const outline           = 5.0f;
    Vec2  const outline_vec       = {outline, outline};
    sf::Color   color             = sf::Color::White;

    Vec2       size;
    Vec2       position;
    Card       background;
    Card       outline_background;
    sf::Font*  font;
    sf::Text   title;
    sf::Text   iteration_idx;
    float      font_scale = 1.0f;

    // AI state
    sf::Text ai_state_label;
    sf::Text ai_state_text;
    sf::CircleShape ai_state;

    // Disturbance state
    sf::Text disturbance_state_label;
    sf::Text disturbance_state_text;
    sf::CircleShape disturbance_state;

    explicit
    SceneState(Vec2 size_, Vec2 position_ = {})
        : size{size_}
        , position{position_}
        , background{size - 2.0f * outline_vec, background_radius, {50, 50, 50}}
        , outline_background{size, background_radius + outline, color}
        , font{&pez::resources::getFont("font")}
        , ai_state{8.0f}
        , disturbance_state{8.0f}
    {
        title.setString("");
        title.setFont(*font);
        title.setCharacterSize(48);
        title.setFillColor(sf::Color::White);

        iteration_idx.setString("");
        iteration_idx.setFont(*font);
        iteration_idx.setCharacterSize(48);
        iteration_idx.setFillColor(sf::Color::White);

        ai_state_label.setString("AI state");
        ai_state_label.setFont(*font);
        ai_state_label.setCharacterSize(16);
        ai_state_label.setFillColor({150, 150, 150});

        ai_state_text.setFont(*font);
        ai_state_text.setCharacterSize(24);
        ai_state_text.setFillColor(sf::Color::White);

        disturbance_state_label = ai_state_label;
        disturbance_state_label.setString("Disturbance");

        disturbance_state_text = ai_state_text;

        setPosition(position);
    }

    void setSize(Vec2 size_)
    {
        size = size_;
        background = {size - 2.0f * outline_vec, background_radius, {50, 50, 50}};
        outline_background = {size, background_radius + outline, color};
    }

    void setColor(sf::Color c)
    {
        color = c;
        outline_background.setColor(c);
    }

    void setPosition(Vec2 position_)
    {
        position                    = position_;
        outline_background.position = position;
        background.position         = position + outline_vec;

        float const margin = outline + 0.75f * padding.x;

        title.setPosition(position + Vec2{margin, 0.35f * padding.y});

        float current_y = title.getPosition().y + 24.0f;
        iteration_idx.setPosition({position_.x + margin, current_y});

        current_y += 40.0f;
        ai_state_label.setPosition(position_.x + margin, current_y);

        current_y += 30.0f;
        ai_state_text.setPosition(position_.x + margin + 30.0f, current_y - 8.0f);
        ai_state.setPosition(position_.x + margin + 5.0f, current_y);

        current_y += 32.0f;
        disturbance_state_label.setPosition(position_.x + margin, current_y);

        current_y += 30.0f;
        disturbance_state_text.setPosition(position_.x + margin + 30.0f, current_y - 8.0f);
        disturbance_state.setPosition(position_.x + margin + 5.0f, current_y);
    }

    void render(pez::render::Context& context)
    {
        // Background
        outline_background.renderHud(context);
        background.renderHud(context);

        // Render title
        title.setScale(font_scale, font_scale);
        context.drawDirect(title);

        // Render AI state
        context.drawDirect(ai_state_label);
        context.drawDirect(ai_state_text);
        context.drawDirect(ai_state);

        // Render disturbance state
        context.drawDirect(disturbance_state_label);
        context.drawDirect(disturbance_state_text);
        context.drawDirect(disturbance_state);
    }

    void setTime(float time)
    {
        title.setString(toString(time, 1) + "s");
    }

    void setAiState(bool state)
    {
        setComponentState(state, ai_state_text, ai_state);
    }

    void setDisturbanceState(bool state)
    {
        setComponentState(state, disturbance_state_text, disturbance_state);
    }

    static void setComponentState(bool state, sf::Text& state_text, sf::CircleShape& state_circle)
    {
        if (state) {
            state_text.setString("Enabled");
            state_circle.setFillColor({138, 177, 125});
        } else {
            state_text.setString("Disabled");
            state_circle.setFillColor({231, 111, 81});
        }
    }
};