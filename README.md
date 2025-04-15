# Unreal MQTT Plugin

This plugin allows using MQTT in Unreal Engine via the Eclipse Paho MQTT C/C++ library.

## Project Overview
UnrealPahoMQTT is a plugin for Unreal Engine that integrates the Eclipse Paho MQTT C/C++ library using its asynchronous non-blocking API to enable MQTT communication directly within Unreal projects.

The plugin is designed to provide a flexible and thread-safe connection to MQTT brokers, with full Blueprint support. Special care has been taken to ensure proper synchronization with the Unreal Game Thread, avoiding common issues when using asynchronous network technologies in real-time engines.

**The plugin offers two levels of usage**:

USimpleMQTTClient: A Blueprint-accessible class that manages one or more MQTT connections. It offers full control over configuration and allows you to subscribe to events, publish messages, and receive data.

MQTT Subsystem: A simplified optional subsystem that can automatically connect to a specified broker at runtime (configured via plugin settings). It provides a much simpler API for common use cases such as sending and receiving messages, accessible via an accompanying Blueprint library.

## Getting Started (Minimal Example)

This plugin offers two ways to use MQTT in Unreal Engine.
The recommended way is via the MQTT Subsystem, which handles connection and message exchange for you.

Here’s a minimal example of how to:

### Receive Messages
Enable the plugin and configure your broker connection in
Project Settings → Plugins → UnrealPahoMQTT.

Create a Blueprint (e.g., BP_MQTT_Receiver) and in BeginPlay:

Bind to the OnMQTTConnected event.

After connection, subscribe to your desired topic using Subscribe to Topic.

Bind to OnMQTTMessageReceived to handle incoming messages.

Example topic:
unreal/example/event

### Send Messages
Create a Blueprint (e.g., BP_MQTT_Sender) and in BeginPlay:

Use Set Timer by Event to call a custom event every few seconds.

In that event, use the Blueprint function Publish Message.

Example topic:
unreal/example/trigger

### Notes
You don’t need to manually create a client – the subsystem does this automatically.
For full control, you can still use USimpleMQTTClient directly.

## License

This plugin is licensed under the MIT License.

It uses the Eclipse Paho MQTT C/C++ library, which is licensed under:
- Eclipse Public License v2.0 (EPL-2.0)
- Eclipse Distribution License v1.0 (EDL-1.0)

You can find the respective license texts in the `ThirdPartyLicenses` folder.
