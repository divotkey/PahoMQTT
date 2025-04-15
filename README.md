# Unreal MQTT Plugin

This plugin allows using MQTT in Unreal Engine via the Eclipse Paho MQTT C/C++ library.

## Project Overview
UnrealPahoMQTT is a plugin for Unreal Engine that integrates the Eclipse Paho MQTT C/C++ library using its asynchronous non-blocking API to enable MQTT communication directly within Unreal projects.

The plugin is designed to provide a flexible and thread-safe connection to MQTT brokers, with full Blueprint support. Special care has been taken to ensure proper synchronization with the Unreal Game Thread, avoiding common issues when using asynchronous network technologies in real-time engines.

**The plugin offers two levels of usage**:

USimpleMQTTClient: A Blueprint-accessible class that manages one or more MQTT connections. It offers full control over configuration and allows you to subscribe to events, publish messages, and receive data.

MQTT Subsystem: A simplified optional subsystem that can automatically connect to a specified broker at runtime (configured via plugin settings). It provides a much simpler API for common use cases such as sending and receiving messages, accessible via an accompanying Blueprint library.

## License

This plugin is licensed under the MIT License.

It uses the Eclipse Paho MQTT C/C++ library, which is licensed under:
- Eclipse Public License v2.0 (EPL-2.0)
- Eclipse Distribution License v1.0 (EDL-1.0)

You can find the respective license texts in the `ThirdParty` folder.
