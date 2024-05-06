# Star Wars Episode I: Racer AP Client

### Now ***this*** is podracing!

This is a randomizer client for **S**tar **W**ars Episode I: **R**acer for the PC. It uses Archipelago (a cross-game randomizer) to shuffle pod parts, race rewards, characters, and the track order.

More information on randomization using the apworld can be found on the [apworld repository](https://github.com/wcolding/SWR_apworld).

### What do I need in order to play this?
You will need the following to play this randomizer:
* An [Archipelago installation](https://github.com/ArchipelagoMW/Archipelago/releases) and the [latest apworld release](https://github.com/wcolding/SWR_apworld/releases) (for generating seeds)
* [The latest release](https://github.com/wcolding/SWR_AP_Client/releases) of this client
* A `dinput.dll (x86)` release of [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases)
* The Windows version of the game. Development has been conducted with the Steam version. The GOG release appears to work but has not been tested extensively.

### How do I use this?
Put the `dinput.dll` from Ultimate ASI Loader into your game's main directory. Put the entire `scripts` folder from the client release in the base folder as well. Run the game and a window should pop up asking if you want to connect to Archipelago. Enter the server address and port (e.g.: `archipelago.gg:12345`), your slot name, and a password if applicable.

#### Note for Steam Deck/Proton/Wine users:
You will need to run the Windows copy of the game with a DLL override in order for the mod to load. You can add `WINEDLLOVERRIDES="dinput=n,b" %command%` to the launch options in the game properties in Steam to do this.

### Anything else I should know?
* You need to create a new profile for each new seed you play
* You need to close the game and restart it for each new seed you play
* Playing multiple slots on the same seed is not currently recommended, but if you wish to do it you will have to carefully manage swapping slots to avoid receiving incorrect items from AP. When you're ready to switch, you should back out to the profile selection screen, pick a previous non-AP profile, and close the game. Then reopen it, connect with your other slot name and create a new separate profile for that
* You can use [dgVoodoo2](https://github.com/dege-diosg/dgVoodoo2/releases) to run the game windowed
* The game contains no text client. To use commands such as hints it's recommended you use the text client included with AP
* Pod parts appear as their original models in the shop. Other items are represented by the following models:
	* Jabba - items for other non-racer worlds
	* Ithorian - circuit pass
	* Dewback - money
 	* Racers - themselves
	* Watto - purchased

### Known Bugs
* The mod tries to prevent you from selecting tracks you shouldn't have access to but sometimes the game may put you on the Invitational Circuit after a race, particularly after Galactic 7
* Progressive Item stat displays may be the incorrect level
* Pod parts for other players currently have stat displays that can be ignored
* On receiving an item during a race the mini map display may move to the upper left for the duration of the text notification
* When using Invitational Circuit Pass the course may not increment after each race like in other cups. The course should still unlock but you may have to manually select it
