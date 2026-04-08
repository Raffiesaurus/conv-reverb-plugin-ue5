# Convolution Reverb Plugin — Unreal Engine 5

A real-time **convolution reverb** plugin built natively for Unreal Engine 5. Implements convolution-based audio processing using Impulse Response (IR) files to simulate physically accurate acoustic spaces directly within MetaSounds and the UE5 audio pipeline.

## About

This plugin was developed as part of research into spatial audio for game environments. Traditional reverb in games relies on algorithmic approaches (delay lines, feedback filters) which are fast but can sound synthetic. Convolution reverb captures the acoustic "fingerprint" of a real space using an IR file and applies it to any audio signal, producing significantly more realistic results.

The plugin exposes a MetaSounds node that accepts an audio signal and an IR asset, performing the FFT-based convolution in real time at runtime. It integrates cleanly with UE5’s audio graph system and supports dynamic IR swapping for environment transitions.

## Features

- Real-time FFT convolution on audio signals
- Custom MetaSounds node integration
- Supports `.wav` Impulse Response assets via UE5 asset pipeline
- Dynamic IR swapping at runtime
- C++ plugin architecture — no Blueprint hacks

## Tech

- **Engine:** Unreal Engine 5
- **Language:** C++
- **Audio System:** MetaSounds / UE5 Audio Mixer
- **Algorithm:** Overlap-add FFT convolution

## Getting Started

1. Clone this repo into your project’s `Plugins/` directory.
2. Regenerate Visual Studio project files.
3. Build the project — the plugin will compile automatically.
4. Enable the plugin in **Edit → Plugins**.
5. In a MetaSounds graph, add the **Convolution Reverb** node and connect your IR asset.

## License

MIT
