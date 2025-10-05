# VolcanoSim (Vesuvius)

An Unreal Engine (UE5) simulation of a volcanic eruption driven by a simple, pressure-based physics model and visualized with Niagara. Parameters (chamber pressure, gas fraction, vent radius) control plume behavior; an interactive UI is being added for real-time tuning.

---

## Project Status

### What’s implemented so far
- Unreal project set up (rebuilt on new laptop) and compiles cleanly  
- World map/level with a volcano location  
- Core physics: pressure-driven exit velocity and mass flow (`v_exit`, `ṁ`) from chamber pressure, gas fraction (φ), vent radius, and densities  
- Niagara plume wired to physics via **User** params:  
  - `User.SpawnRate` → emitter Spawn Rate  
  - `User.Velocity` (or `User.UpVelocity`) → vertical velocity  
- Debug overlay prints live values (φ, radius, ρ_mix, v, ṁ)

### What’s still to come
- UMG UI (sliders) to adjust pressure/φ/radius live  
- Polishing Niagara look (noise, drag, LODs, materials)  
- Debris/rocks via Chaos; heat glow material; multi-burst pulses

### Changes from original proposal
- Narrowed scope for Milestone 2 to a **pressure-based plume model** first for testing before further development  
- Shifted from Blueprint-only to **C++ + Niagara + (soon) UMG** for repeatability and easier debugging

---

## Installation Instructions

### Dependencies & versions (tested)
- **Unreal Engine 5.x** (tested on 5.3/5.4)  
- **Windows 10/11** with **Visual Studio 2022** (“Desktop development with C++”)  
- Plugins: **Niagara** (on), **UMG** (on). Optional: **Modeling Tools**, **Chaos** **Landmass** **Water**

### Step-by-step setup
1. **Clone** the repo and open the folder with the `.uproject`.  
2. **Windows**: right-click the `.uproject` → **Generate Visual Studio project files**.  
3. Open the `.sln` and **Build**, or open the `.uproject` in UE and click **Compile**.  
4. In UE: **Edit → Plugins** → ensure **Niagara** and **UMG** are enabled.  
5. Open your Niagara **System** (e.g., `NS_VolcanoPlume`) and configure once:
   - Add **User Float** parameters: `SpawnRate`, `Velocity`  
   - Bind **SpawnRate** → **Spawn Rate → Rate**  
   - Bind **Velocity** → **Initialize/Add Velocity → Z**  
   - In **System Settings**: enable **Use Fixed Bounds**; set **Min** `(-5000,-5000,0)`, **Max** `(5000,5000,9000)`; **Save**

## Usage

### How to run the simulation (fast path)
1. **Place the actor**: **Window → Place Actors**, search `VesuviusOneBuild` (or use `BP_Volcano` if you made one), drag into the level.  
2. **Assign Niagara**: select the actor → set the `PlumeSystem` property (or set the Niagara System on the `PlumeFX` component).  
3. **Start**: either call `StartEruption(60)` on **BeginPlay** in the Level Blueprint, or enable `bAutoStart` in the actor (if present).  
4. **Play**: you should see a rising smoke plume; on-screen debug prints physics values.

### Expected output/behavior
- GPU-sprite Niagara plume that grows/accelerates with higher pressure or larger vent radius and varies density with gas fraction  
- Debug overlay shows φ, r, ρ_mix, v, ṁ  
- With UI (when merged): sliders adjust the eruption in real time

---

## Architecture Overview

### Main components
- **Simulation Actor**: `AVesuviusOneBuild`  
  - Computes `ρ_gas`, `ρ_mix`, `v_exit`, `ṁ` each tick  
  - Calls `PlumeFX->SetFloatParameter("User.SpawnRate", …)` and `("User.Velocity", …)`
- **Visualization**: `NS_VolcanoPlume` (Niagara System)  
  - GPU sprite emitter; Spawn Rate ← mass flow; vertical velocity ← exit velocity; noise/drag for mushrooming
- **UI (in progress)**: `WBP_VolcanoUI`  
  - Sliders call `SetChamberPressure`, `SetGasFraction`, `SetVentRadius` on the actor
- **Orchestration**: Level Blueprint or `VesuviusOneGameModeBase`  
  - Starts eruptions on BeginPlay; can spawn actors programmatically

### Mapping to UML design
- **Entity**: Volcano (Actor) – state + physics methods  
- **Controller**: UI Widget – updates parameters; triggers start/stop  
- **View**: Niagara System – renders plume

### Architectural changes & why
- From “Blueprint-heavy” to **C++ core + Niagara + thin UI** → simpler debugging, determinism for milestones  
- Deferred advanced multiphase/debris features to keep the milestone deliverable stable

---

## Repo Layout (key paths)
