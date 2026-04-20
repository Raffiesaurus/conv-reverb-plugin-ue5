# ReverbShift - Convolution Reverb Plugin for UE5

A real-time **convolution reverb** plugin built natively for Unreal Engine 5. Implements FFT-based audio convolution using Impulse Response (IR) files to simulate physically accurate acoustic spaces, exposed as a native **MetaSounds node**.

---

## How It Works

Traditional game reverb uses algorithmic approaches — delay lines, feedback filters — which are fast but can sound synthetic. Convolution reverb captures the acoustic fingerprint of a real space via an IR file and applies it to any audio signal using FFT overlap-add convolution, producing significantly more realistic results.

ReverbShift plugs directly into UE5's MetaSounds graph: connect an audio signal and an IR asset, and you get real-time convolved reverb at runtime with support for dynamic IR swapping on environment transitions.

---

## Features

- Real-time FFT overlap-add convolution at runtime
- Native MetaSounds node — works inside any MetaSounds graph
- Accepts `.wav` Impulse Response assets via UE5's asset pipeline
- Dynamic IR swapping for seamless environment transitions
- Pure C++ plugin — no Blueprint workarounds

---

## Getting Started

### Installation

1. Copy the `Plugins/ReverbShift/` directory into your project's `Plugins/` folder
2. Right-click your `.uproject` → **Generate Visual Studio project files**
3. Build the project — the plugin compiles automatically with your game module
4. Enable it in **Edit → Plugins → ReverbShift**

### Usage in MetaSounds

1. Open a MetaSounds asset (or create one)
2. Right-click the graph → search for **Convolution Reverb** (or **ReverbShift**)
3. Connect your audio input and an IR wave asset
4. Wire the output into your MetaSounds output node
5. Swap IR assets at runtime via Blueprint or C++ to change acoustic environments

---

## Tech

- **Engine:** Unreal Engine 5
- **Language:** C++
- **Audio system:** MetaSounds / UE5 Audio Mixer
- **Algorithm:** Overlap-add FFT convolution
- **Plugin name:** `ReverbShift`

---

## Project Structure
```
Plugins/
ReverbShift/
Source/                       # C++ plugin source — MetaSounds node, FFT processing
Content/                      # Sample IR assets (if included)
ReverbShift.uplugin
ConvReverbShiftDev.uproject   # Dev/test UE5 project
```

---

## License

MIT
