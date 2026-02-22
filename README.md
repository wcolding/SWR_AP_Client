# Star Wars Episode I: Racer AP Client

### Now ***this*** is podracing!

This is a randomizer client for **S**tar **W**ars Episode I: **R**acer for the PC. It uses Archipelago (a cross-game randomizer) to shuffle pod parts, race rewards, characters, and the track order.

More information on randomization using the apworld can be found on the [apworld repository](https://github.com/wcolding/SWR_apworld).

### What do I need in order to play this?
You will need the following to play this randomizer:
* An [Archipelago installation](https://github.com/ArchipelagoMW/Archipelago/releases)
* [The latest release](https://github.com/wcolding/SWR_AP_Client/releases) of this client and apworld
* The Windows version of the game. Development has been conducted with the Steam and GOG versions
* The [x86 Visual C++ Redistributable](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist) if not already installed

### How do I use this?
1. Place `dsound.dll`, `APCpp.dll`, and `SWR_AP_Client.ini` into your game's main directory
2. Run the game and a window should pop up asking if you want to connect to Archipelago. Enter the server address and port (e.g.: `archipelago.gg:12345`), your slot name, and a password if applicable.
3. Click `Log In`. If all goes well you should see "Connected to AP" and a seed number in the upper right hand corner of the game

#### Note for Steam Deck/Proton/Wine users:
You will need to run the Windows copy of the game with a DLL override in order for the mod to load. You can add `WINEDLLOVERRIDES="dinput=n,b" %command%` to the launch options in the game properties in Steam to do this. If using `dsound`, put that instead. **As of client version 0.7.0 you may also need to install `vcrun2022` using Protontricks/Winetricks.**

### Anything else I should know?
* You need to create a new profile for each new seed you play. The client creates new save folders per seed but you will still need to start a new profile to play
* You need to close the game and restart it for each new seed/slot you play
* Playing multiple Racer slots on the same seed is not currently recommended
* The game contains no text client. To use commands such as hints it's recommended you use the text client included with AP
* Pod parts appear as their original models in the shop. Other items are represented by the following models:
	* Jabba - items for other non-Racer worlds
	* Ithorian - circuit pass / course unlocks
	* Dewback - money
 	* Racers - themselves
	* Watto - purchased

### Known Bugs
* Progressive Item stat displays may be the incorrect level
* Pod parts for other players currently have stat displays that can be ignored
* When using Invitational Circuit Pass the course may not increment after each race like in other cups. The course should still unlock but you may have to manually select it

### General tips for running this old game on modern hardware
**Windowed mode** - You can use [dgVoodoo2](https://github.com/dege-diosg/dgVoodoo2/releases). You should move the 4 dlls out of `MS\x86` into your game directory and then place `dgVoodoo.conf` and `dgVoodooCpl.exe` into that same directory. Run `dgVoodooCpl.exe` and change the `Appearance` setting to windowed. You will also want to go to the DirectX tab and uncheck `Application controlled fullscreen/windowed state`, `Disable Alt-Enter to toggle screen state`, and `dgVoodoo Watermark`.

**Device enumeration errors / crashes** - You may need to use a dinput wrapper that translates the calls into dinput8 ones. [dinputto8](https://github.com/elishacloud/dinputto8/releases) has been helpful for some people in getting this game running on Windows 10 and 11 devices.
