#include <iostream>
#include <chrono>
#include <thread>
#include "../../src/runtime/embedder.hpp"
#include "../../src/runtime/input-manager/input_manager.hpp"

/**
 * Example: Keyboard Event Injection Demo
 * 
 * This example demonstrates how a host application can inject keyboard events
 * into JSAR Runtime using the InputManager module.
 */

class KeyboardInputDemo : public TrEmbedder
{
public:
    KeyboardInputDemo() : TrEmbedder(TrHostEngine::None) {}

    bool onEvent(events_comm::TrNativeEvent &event, std::shared_ptr<TrContentRuntime> content) override
    {
        // Handle events from content runtime
        return true;
    }

    void runDemo()
    {
        std::cout << "JSAR Runtime - Keyboard Input Injection Demo\n";
        std::cout << "=============================================\n\n";

        // Configure and start the runtime
        if (!configure("/tmp/jsar-demo", "", false))
        {
            std::cerr << "Failed to configure JSAR Runtime\n";
            return;
        }

        if (!start())
        {
            std::cerr << "Failed to start JSAR Runtime\n";
            return;
        }

        std::cout << "JSAR Runtime started successfully.\n";
        std::cout << "InputManager initialized.\n\n";

        // Simulate host application injecting keyboard events
        std::cout << "Simulating keyboard event injection...\n";

        // Example 1: Single key press simulation
        simulateKeyPress();

        // Example 2: Text typing simulation
        simulateTextTyping("Hello JSAR!");

        // Example 3: Modifier key combinations
        simulateModifierCombinations();

        std::cout << "\nDemo completed. Shutting down...\n";
        shutdown();
    }

private:
    void simulateKeyPress()
    {
        std::cout << "\n1. Simulating single key press (Enter key):\n";
        
        auto keyDownEvent = input_manager::keyboard_events::createKeyDownEvent(
            "Enter", "Enter", false, false, false, false);
        
        auto keyUpEvent = input_manager::keyboard_events::createKeyUpEvent(
            "Enter", "Enter", false, false, false, false);

        // Broadcast to all active content runtimes
        int contentCount = broadcastKeyboardEvent(keyDownEvent);
        std::cout << "   Key down sent to " << contentCount << " content runtime(s)\n";
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        contentCount = broadcastKeyboardEvent(keyUpEvent);
        std::cout << "   Key up sent to " << contentCount << " content runtime(s)\n";
    }

    void simulateTextTyping(const std::string& text)
    {
        std::cout << "\n2. Simulating text typing: \"" << text << "\"\n";
        
        for (char c : text)
        {
            std::string key(1, c);
            std::string code = getCodeForChar(c);
            
            auto keyDownEvent = input_manager::keyboard_events::createKeyDownEvent(
                key, code, false, false, false, false);
            
            auto keyUpEvent = input_manager::keyboard_events::createKeyUpEvent(
                key, code, false, false, false, false);

            int contentCount = broadcastKeyboardEvent(keyDownEvent);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            broadcastKeyboardEvent(keyUpEvent);
            
            std::cout << "   Typed '" << c << "' to " << contentCount << " content(s)\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void simulateModifierCombinations()
    {
        std::cout << "\n3. Simulating modifier key combinations (Ctrl+A):\n";
        
        // Ctrl key down
        auto ctrlDown = input_manager::keyboard_events::createKeyDownEvent(
            "Control", "ControlLeft", false, true, false, false);
        broadcastKeyboardEvent(ctrlDown);
        std::cout << "   Ctrl key down\n";
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        // A key down (with Ctrl modifier active)
        auto aDown = input_manager::keyboard_events::createKeyDownEvent(
            "a", "KeyA", false, true, false, false);
        broadcastKeyboardEvent(aDown);
        std::cout << "   A key down (with Ctrl modifier)\n";
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        // A key up
        auto aUp = input_manager::keyboard_events::createKeyUpEvent(
            "a", "KeyA", false, true, false, false);
        broadcastKeyboardEvent(aUp);
        std::cout << "   A key up\n";
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        // Ctrl key up
        auto ctrlUp = input_manager::keyboard_events::createKeyUpEvent(
            "Control", "ControlLeft", false, false, false, false);
        int contentCount = broadcastKeyboardEvent(ctrlUp);
        std::cout << "   Ctrl key up - Ctrl+A sent to " << contentCount << " content(s)\n";
    }

    std::string getCodeForChar(char c)
    {
        if (c >= 'a' && c <= 'z')
            return "Key" + std::string(1, c - 'a' + 'A');
        if (c >= 'A' && c <= 'Z')
            return "Key" + std::string(1, c);
        if (c == ' ')
            return "Space";
        if (c >= '0' && c <= '9')
            return "Digit" + std::string(1, c);
        
        return "Unknown";
    }
};

int main()
{
    try
    {
        KeyboardInputDemo demo;
        demo.runDemo();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Demo failed with exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}