#pragma once
#include <map>
#include <functional>
#include <iostream>

#include "SFML/Window/Window.hpp"


namespace pez
{

/// The base object that will be used to check compatibility
struct EventCallbackBase
{
    virtual ~EventCallbackBase() = default;
    virtual void tryProcess(sf::Event const& event_raw) const = 0;
};

/// The actual callback that will be used given an event type
template<typename TEvent>
struct EventCallback final : public EventCallbackBase
{
private:
    using Callback = std::function<void(TEvent const&)>;
    Callback m_callback{nullptr};

public:
    explicit
    EventCallback(Callback callback)
        : m_callback{std::move(callback)}
    {}

    void tryProcess(sf::Event const& event_raw) const override
    {
        auto const* event = event_raw.getIf<TEvent>();
        if (event) {
            m_callback(*event);
        }
    }
};

template<typename TKey, typename TEvent>
class EventCategoryHandler
{
public:
    using CallbackEvent = std::function<void(TEvent)>;
    using CallbackKey = std::function<TKey(TEvent const&)>;

private:
    std::unordered_map<TKey, CallbackEvent> m_maps;
    CallbackKey m_key_extractor;

public:
    explicit
    EventCategoryHandler(CallbackKey key_extractor)
        : m_key_extractor{std::move(key_extractor)}
    {}

    void setEventCallback(TKey key, CallbackEvent callback)
    {
        m_maps[key] = std::move(callback);
    }

    void handle(TEvent const& event)
    {
        auto const it{m_maps.find(m_key_extractor(event))};
        if (it != m_maps.end()) {
            it->second(event);
        }
    }
};

/**
 *
 */
class EventHandler
{
    using KeyPressedHandler = EventCategoryHandler<sf::Keyboard::Key, sf::Event::KeyPressed>;
    using KeyReleasedHandler = EventCategoryHandler<sf::Keyboard::Key, sf::Event::KeyReleased>;
    using MousePressedHandler = EventCategoryHandler<sf::Mouse::Button, sf::Event::MouseButtonPressed>;
    using MouseReleasedHandler = EventCategoryHandler<sf::Mouse::Button, sf::Event::MouseButtonReleased>;

    using MouseMovedCallbackEvent = std::function<void(sf::Event::MouseMoved const&)>;
    using MouseWheelScrolledCallbackEvent = std::function<void(sf::Event::MouseWheelScrolled const&)>;

public:
    explicit
    EventHandler(sf::Window& window)
        : m_window{&window}
        , m_key_press_handler([](sf::Event::KeyPressed const& e) { return e.code; })
        , m_key_released_handler([](sf::Event::KeyReleased const& e) { return e.code; })
        , m_mouse_pressed_handler([](sf::Event::MouseButtonPressed const& e) { return e.button; })
        , m_mouse_released_handler([](sf::Event::MouseButtonReleased const& e) { return e.button; })
    {
        addCallback<sf::Event::KeyPressed>([this](sf::Event::KeyPressed const& e) {
            m_key_press_handler.handle(e);
        });

        addCallback<sf::Event::KeyReleased>([this](sf::Event::KeyReleased const& e) {
            m_key_released_handler.handle(e);
        });

        addCallback<sf::Event::MouseButtonPressed>([this](sf::Event::MouseButtonPressed const& e) {
            m_mouse_pressed_handler.handle(e);
        });

        addCallback<sf::Event::MouseButtonReleased>([this](sf::Event::MouseButtonReleased const& e) {
            m_mouse_released_handler.handle(e);
        });
    }

    template<typename TEvent>
    void addCallback(std::function<void(TEvent const&)> callback)
    {
        m_event_callbacks.push_back(std::make_unique<EventCallback<TEvent>>(std::move(callback)));
    }

    void processEvents() const
    {
        while (std::optional<sf::Event> const& event = m_window->pollEvent()) {
            if (event.has_value()) {
                sf::Event const& valid_event{*event};
                for (auto const& callback : m_event_callbacks) {
                    callback->tryProcess(valid_event);
                }
            }
        }
    }

    void onKeyPressed(sf::Keyboard::Key const key_code, KeyPressedHandler::CallbackEvent callback)
    {
        m_key_press_handler.setEventCallback(key_code, std::move(callback));
    }

    void onMousePressed(sf::Mouse::Button const button, MousePressedHandler::CallbackEvent callback)
    {
        m_mouse_pressed_handler.setEventCallback(button, std::move(callback));
    }

    void onMouseReleased(sf::Mouse::Button const button, MouseReleasedHandler::CallbackEvent callback)
    {
        m_mouse_released_handler.setEventCallback(button, std::move(callback));
    }

    void onMouseMoved(MouseMovedCallbackEvent callback)
    {
        addCallback<sf::Event::MouseMoved>(std::move(callback));
    }

    void onMouseWheelScrolled(MouseWheelScrolledCallbackEvent callback)
    {
        addCallback<sf::Event::MouseWheelScrolled>(std::move(callback));
    }

private:
    sf::Window* m_window = nullptr;

    KeyPressedHandler m_key_press_handler;
    KeyReleasedHandler m_key_released_handler;
    MousePressedHandler m_mouse_pressed_handler;
    MouseReleasedHandler m_mouse_released_handler;

    std::vector<std::unique_ptr<EventCallbackBase>> m_event_callbacks;
};

}
