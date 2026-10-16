# Auto Deco

**Made by XYS**

Auto Deco is a Geode mod for the Geometry Dash level editor. It does two jobs:

1. **Auto Deco** decorates your layout for you. Pick a theme and a mood, tap one button, and deco is placed along the top and underside of your blocks, in the background, and on the beat of your song.
2. **Layout Generator** builds a brand-new cube layout from Easy up to Demon. Every layout is different, and every layout can be recreated with its seed.

Both tools live in the editor, right next to the Trash button.

---

## Contents

- [Quick start](#quick-start)
- [Where are the buttons?](#where-are-the-buttons)
- [Auto Deco in detail](#auto-deco-in-detail)
- [Tips for deco that looks great](#tips-for-deco-that-looks-great)
- [Layout Generator in detail](#layout-generator-in-detail)
- [Installing](#installing)
- [Updating](#updating)
- [Troubleshooting](#troubleshooting)
- [Project structure](#project-structure)
- [Publishing for everyone](#publishing-for-everyone)
- [Roadmap](#roadmap)

---

## Quick start

**Decorate a layout**

1. Open the level editor and build a layout (blocks and spikes).
2. Place a handful of deco objects you like (stars, saws, chains, whatever fits your theme).
3. Select those deco objects, tap the green **Deco** button, choose a **Theme**, then tap **Edge** to save them. Do the same for **Hang**, **BG** and **Beat** if you want different objects for each job.
4. Tap **Generate Deco**.

**Generate a layout**

1. Tap the blue **Gen** button.
2. Choose **Difficulty**, **Length** and **Speed**.
3. Tap **Generate Layout**, then playtest it.

---

## Where are the buttons?

Two round buttons sit at the top-left of the editor, in the same row as Undo, Redo and Trash:

| Button | Color | Opens |
|---|---|---|
| **Deco** | green | the Auto Deco window |
| **Gen** | blue | the Layout Generator window |

Each window has a small **i** button in the corner that explains every option.

---

## Auto Deco in detail

### Theme
A theme is a folder for your saved deco. There are five: **Space, Slaughter, Neon, Forest, Cyber**. Each theme remembers its own palette, so you can build a Space palette once and switch between themes freely.

### Mood
The mood controls how the deco behaves, not what it looks like.

| Mood | Character |
|---|---|
| **Clean** | Tidy and sparse accents. Great for modern and glow styles. |
| **Slaughterhouse** | Dense and chaotic, lots of hanging pieces and tilted deco. |
| **Limbo** | Very sparse, with huge silhouettes in the background. |
| **Dreamy** | Soft and floaty, large background pieces with a slight tilt. |

### Density
**Low, Medium, High.** Scales how many pieces are placed. Start with Medium. Use Low on small layouts, because too much deco hides the gameplay.

### Beat Sync
Turn it on, type the **BPM** of your song, and pick the **Speed** your level uses.

- Big background pieces are placed on every bar (every 4 beats).
- Two rails of accents (above and below the layout) are placed on the beats. The first beat of each bar is bigger.
- Low density only marks the bars, Medium marks every second beat, High marks every beat.

Beat Sync assumes your song starts at the start of the level and that the speed does not change. If the deco looks a little off from the music, select it all and nudge it left or right.

### Palette (the important part)
Auto Deco does not guess which objects look good. You choose them:

1. Place or select some **deco** objects in the editor.
2. Open the Deco window and pick the theme they belong to.
3. Tap a slot:
   - **Edge**: small pieces placed on top of block surfaces (grass, thorns, lights).
   - **Hang**: pieces hanging under blocks (chains, hooks, stalactites).
   - **BG**: big pieces for the background (planets, towers, silhouettes).
   - **Beat**: accents placed on the beat.
4. **Reset** clears the palette of the current theme.

Rules to know:
- You need at least the **Edge** slot filled. Empty slots borrow from other slots.
- Only real decoration objects are accepted. If you select a block or a spike, the mod ignores it.
- The same safety applies when generating: anything that is not decoration is removed again immediately, so Auto Deco cannot add objects that kill the player.

### After generating
Everything that was added stays **selected**. That means you can:

- tap **Edit Object** and change the color channel to recolor all of it at once,
- change the **Z layer** of everything at once,
- press **Trash** to undo the whole thing and try again.

If you select part of your layout before generating, only that part is decorated. If you select nothing (or only deco), the whole level is used.

---

## Tips for deco that looks great

These are common habits of good decorators (see the GD Creator School deco guide and community guides). Auto Deco gives you the placement. These steps give you the polish:

1. **Work in passes.** Shapes and colors first, details after. Generate once, recolor, then add details by hand.
2. **Use a tiny palette.** Two or three colors is plenty. Strong pairs: black and white, black and red, dark blue and purple.
3. **Keep gameplay readable.** Deco should be darker or softer than blocks and spikes, never brighter. Put background pieces on back Z layers.
4. **One idea per Z layer.** Background on B4 or B3, mid pieces on B2, accents closer to the front. Mixed layers look muddy.
5. **Add glow.** Duplicate key pieces, enlarge them, lower the opacity, and use additive blending.
6. **Make it move.** Pulse triggers on the beat make deco feel alive. Auto Deco places the objects on the beat, you add the pulses.
7. **Do not overdecorate.** Empty space makes the busy parts look better.

Quick recipes:

- **Slaughterhouse look:** Slaughter theme, Slaughterhouse mood, save saws, chains, hooks and skulls. After generating, set the color channel to a deep red.
- **Space look:** Space theme, Clean or Dreamy mood, save stars, gears and glowing blocks. Recolor to white and purple, background pieces dark.
- **Limbo look:** Any theme, Limbo mood, Low density. Make the huge pieces black on a grey background.

---

## Layout Generator in detail

Builds a cube layout from a library of jump patterns: single, double and triple spikes, platforms, stairs, spikes on platforms, yellow pad jumps, and double-single rhythms.

### Difficulty

| Difficulty | What you get |
|---|---|
| **Easy** | Wide gaps with single or double spikes. Relaxed timing. |
| **Normal** | Adds platforms, stairs and spikes on top of platforms. |
| **Hard** | Triple spikes, double-single rhythms and the odd pad jump. |
| **Harder** | Tighter rhythm, more combos and more pads. |
| **Insane** | Dense combos with short breathers. Needs fast reactions. |
| **Demon** | Near-constant action with the tightest gaps this tool allows. |

### Length
**Tiny (~15 s), Short (~30 s), Medium (~60 s), Long (~90 s).** This is real level time at the chosen speed.

### Speed
**1x, 2x, 3x, 4x.** A speed portal is placed at the start. Gaps scale with the speed so the layout stays fair.

### Seed
Leave it empty for a random layout. After generating, the seed that was used is written into the box. Type the same seed again to get the same layout back, or share it with a friend.

### Why every layout is different
Each generation picks a random **motif** (three favorite patterns that get boosted), random gaps inside the difficulty range, random pattern order, and never repeats the same pattern three times in a row.

### How it stays fair
For a cube at normal speed, one jump covers about 4 blocks and rises about 2. The generator never places obstacle groups closer than the distance needed to land and jump again, and wider spike groups get more room. All distances scale with speed.

### What to know
- The layout is **not playtested** by the tool. Always playtest it. If a spot feels wrong, generate again or tweak it by hand.
- Only the **cube** gamemode is generated for now.
- New content is added **after** anything already in your level, so you can generate several sections in a row.
- Everything stays selected after generating so you can move or delete it as one piece.
- Add your own end trigger when you are done.

---

## Installing

### Android (phone only, with Termux)
Building Geode mods on a phone is heavy, so GitHub builds it for you for free.

```bash
# one-time setup
pkg update -y && pkg install -y git gh unzip
termux-setup-storage

# unpack the project
cd ~ && unzip ~/storage/downloads/AutoDeco-v1.1.zip && cd ~/AutoDeco

# upload to your own GitHub repo (log in with the browser flow)
git init -b main && git add . && git -c user.name=XYS -c user.email=xys@users.noreply.github.com commit -m "Auto Deco"
gh auth login
gh auth setup-git
git remote add origin https://github.com/YOUR-USERNAME/autodeco.git
git push -u origin main

# wait for the build (about 3 to 10 minutes), then download it
gh run watch
gh run download -n "Build Output"

# install into Geode
cp *.geode ~/storage/shared/Android/media/com.geode.launcher/game/geode/mods/
```

Restart Geometry Dash through the Geode launcher.

### Desktop
1. Install the Geode CLI and SDK: https://docs.geode-sdk.org/getting-started/
2. Run `geode build` inside the project folder. It builds and installs the mod.

---

## Updating

```bash
cd ~ && unzip -o ~/storage/downloads/AutoDeco-v1.1.zip
cd ~/AutoDeco
git add . && git -c user.name=XYS -c user.email=xys@users.noreply.github.com commit -m "Update" && git push
gh run watch
rm -f *.geode && gh run download -n "Build Output"
cp *.geode ~/storage/shared/Android/media/com.geode.launcher/game/geode/mods/
```

Fully close Geometry Dash before copying the new file, then open it again.

---

## Troubleshooting

| Problem | Fix |
|---|---|
| The buttons do not appear | Make sure Auto Deco is enabled in the Geode mod list, and restart the game. The mod is built for Geode 5.x. |
| The buttons are in the wrong spot | Tell me your screen and I will adjust. The position is calculated from the Undo row in `src/main.cpp`. |
| "Your palette is empty" | Select deco objects in the editor and tap Edge, Hang, BG or Beat first. |
| "Select some DECO objects" | What you selected is a block or a spike. Pick real decoration objects. |
| Deco looks like spikes | Those are decoration objects shaped like spikes from your palette. They cannot hurt the player. Tap Reset and save different objects. |
| Nothing was placed | The saved objects were rejected as non-decoration. Save new ones. |
| A layout feels impossible | Playtest and regenerate. Please send me the seed and difficulty so I can improve the patterns. |
| The build fails on GitHub | Open the failed run and send me the red error lines. |

---

## Project structure

```
AutoDeco/
|-- mod.json                 mod info
|-- logo.png                 mod logo
|-- about.md, changelog.md   shown on the Geode mod page
|-- CMakeLists.txt           build file
|-- .github/workflows/       automatic GitHub build
`-- src/
    |-- main.cpp             adds the Deco and Gen buttons to the editor
    |-- DecoPopup.hpp        Auto Deco window
    |-- LayoutPopup.hpp      Layout Generator window
    |-- Engine.hpp           deco placement engine
    |-- LayoutGen.hpp        layout patterns and generator
    |-- UiKit.hpp            small UI helpers
    `-- Themes.hpp           themes, moods, speeds (easy to extend)
```

Want a new theme? Add one line to `kThemes` in `Themes.hpp` and bump the array size.

---

## Publishing for everyone

The mod is publish-ready: it has a logo, an about page, a changelog, a repository link and full metadata. To list it for all players, it has to be submitted to the **Geode Index**, where the Geode team reviews it before it goes live. Check the current rules at https://docs.geode-sdk.org before submitting. Keeping it private (just for you) works fine too.

---

## Roadmap

- Automatic colors per theme
- Pulse triggers on every beat
- Beat detection straight from the song
- Ship, ball, UFO and wave sections in the Layout Generator
- Orb patterns
- Speed changes mid-level
- Layout Generator and Auto Deco in one tap

---

**Made by XYS**
