# Changelog

## [2.4.0](https://launchpad.net/mixxx/+milestone/2.4.0) (Unreleased)

* Cover art: Prevent wrong cover art display due to hash conflicts
* Cover art: Add background color for quick cover art preview
* Add Random Track Control to AutoDJ [#3076](https://github.com/mixxxdj/mixxx/pull/3076)
* Add support for saving loops as hotcues [#2194](https://github.com/mixxxdj/mixxx/pull/2194) [lp:1367159](https://bugs.launchpad.net/mixxx/+bug/1367159)

## [2.3.1](https://launchpad.net/mixxx/+milestone/2.3.1) (Unreleased)

* Added mapping for the Numark DJ2GO Touch controller [#4108](https://github.com/mixxxdj/mixxx/pull/4108)
* Add support for HiDPI scale factors of 125% and 175% (only with Qt 5.14+) [lp1938102](https://bugs.launchpad.net/mixxx/+bug/1938102) [#4161](https://github.com/mixxxdj/mixxx/pull/4161)
* Fix unhandled exception when parsing corrupt Rekordbox PDB files [lp1933853](https://bugs.launchpad.net/mixxx/+bug/1933853) [#4040](https://github.com/mixxxdj/mixxx/pull/4040)
* Fix Echo effect adding left channel samples to right channel [#4141](https://github.com/mixxxdj/mixxx/pull/4141)
* Fix bad phase seek when starting from preroll [lp1930143](https://bugs.launchpad.net/mixxx/+bug/1930143) [#4093](https://github.com/mixxxdj/mixxx/pull/4093)
* Fix bad phase seek when a channel's audible status changes [#4156](https://github.com/mixxxdj/mixxx/pull/4156)
* Tango skin: Show crossfader assign buttons by default [#4046](https://github.com/mixxxdj/mixxx/pull/4046)
* Fix keyfinder library in arm64 builds [#4047](https://github.com/mixxxdj/mixxx/pull/4047)
* Fix wrong track being recorded in History [lp1933991](https://bugs.launchpad.net/mixxx/+bug/1933991) [#4041](https://github.com/mixxxdj/mixxx/pull/4041) [#4059](https://github.com/mixxxdj/mixxx/pull/4059) [#4107](https://github.com/mixxxdj/mixxx/pull/4107)
* Fix support for relative paths in the skin system which caused missing images in third-party skins [#4151](https://github.com/mixxxdj/mixxx/pull/4151)
* Fix relocation of directories with special/reserved characters in path name [#4146](https://github.com/mixxxdj/mixxx/pull/4146)
* Update keyboard shortcuts sheet [#4042](https://github.com/mixxxdj/mixxx/pull/4042)
* Library: resize the Played checkbox and BPM lock with the library font [#4050](https://github.com/mixxxdj/mixxx/pull/4050)
* Don't allow Input focus on waveforms [#4134](https://github.com/mixxxdj/mixxx/pull/4134)
* Fix performance issue on AArch64 by enabling flush-to-zero for floating-point arithmetic [#4144](https://github.com/mixxxdj/mixxx/pull/4144)
* Fix custom key notation not restored correctly after restart [#4136](https://github.com/mixxxdj/mixxx/pull/4136)
* Traktor S3: Disable scratch when switching decks to prevent locked scratch issue [#4073](https://github.com/mixxxdj/mixxx/pull/4073)

### Packaging

* It is no longer necessary to manually copy the udev rule file in packaging scripts. Now pkg-config is used to determine the udevdir used to install the rules file in the CMake install step when CMAKE_INSTALL_PREFIX is `/` or `/usr`.  [#4126](https://github.com/mixxxdj/mixxx/pull/4126)
* Various build issues on FreeBSD are fixed [#4122](https://github.com/mixxxdj/mixxx/pull/4122) [#4123](https://github.com/mixxxdj/mixxx/pull/4123) [#4124](https://github.com/mixxxdj/mixxx/pull/4124)
* .desktop file has be renamed to org.mixxx.Mixxx.desktop according to Freedesktop standards [#4206](https://github.com/mixxxdj/mixxx/pull/4206)
* Uses system provided hidapi library if version >= 0.10.1 [#4215](https://github.com/mixxxdj/mixxx/pull/4215)
* Please update PortAudio to [19.7](https://github.com/PortAudio/portaudio/releases/tag/v19.7.0) if you have not done so already. This is required for Mixxx to work with PipeWire via the JACK API for many devices.

## [2.3.0](https://launchpad.net/mixxx/+milestone/2.3.0) (2021-06-28)

### Hotcues

* Add hotcue colors and custom labels by right clicking hotcue buttons or right clicking hotcues on overview waveforms [#2016](https://github.com/mixxxdj/mixxx/pull/2016) [#2520](https://github.com/mixxxdj/mixxx/pull/2520) [#2238](https://github.com/mixxxdj/mixxx/pull/2238) [#2560](https://github.com/mixxxdj/mixxx/pull/2560) [#2557](https://github.com/mixxxdj/mixxx/pull/2557) [#2362](https://github.com/mixxxdj/mixxx/pull/2362)
* Mouse hover cues on overview waveform to show time remaining until the cue [#2238](https://github.com/mixxxdj/mixxx/pull/2238)

### Hotcue & Track Colors

* Add configurable color per track [#2470](https://github.com/mixxxdj/mixxx/pull/2470) [#2539](https://github.com/mixxxdj/mixxx/pull/2539) [#2545](https://github.com/mixxxdj/mixxx/pull/2545) [#2630](https://github.com/mixxxdj/mixxx/pull/2630) [lp:1100882](https://bugs.launchpad.net/mixxx/+bug/1100882)
* Add customizable color palettes for hotcue and track colors [#2530](https://github.com/mixxxdj/mixxx/pull/2530) [#2589](https://github.com/mixxxdj/mixxx/pull/2589) [#3749](https://github.com/mixxxdj/mixxx/pull/3749) [#2902](https://github.com/mixxxdj/mixxx/pull/2902)
* Add hotcue color find-and-replace tool [#2547](https://github.com/mixxxdj/mixxx/pull/2547)

### Importing From Other DJ Software

* Import cue points, track colors, and playlists from Serato file tags & database [#2480](https://github.com/mixxxdj/mixxx/pull/2480) [#2526](https://github.com/mixxxdj/mixxx/pull/2526) [#2499](https://github.com/mixxxdj/mixxx/pull/2499) [#2495](https://github.com/mixxxdj/mixxx/pull/2495) [#2673](https://github.com/mixxxdj/mixxx/pull/2673) [#3885](https://github.com/mixxxdj/mixxx/pull/3885)
  * Note: Mixxx does not yet support multiple loops per track. We are [working on this for Mixxx 2.4](https://github.com/mixxxdj/mixxx/pull/2194). In Mixxx 2.3, if you import a track with multiple loops from Serato, Mixxx will use the first loop cue as the single loop Mixxx currently supports. The imported loops are still stored in Mixxx's database and are treated as hotcues in Mixxx 2.3. If you do not delete these hotcues, they will be usable as loops in Mixxx 2.4. Serato keeps loops and hotcues in separate lists, but Mixxx does not, so loops from Serato are imported starting as hotcue 9.
* Import cue points, track colors, and playlists from Rekordbox USB drives [#2119](https://github.com/mixxxdj/mixxx/pull/2119) [#2555](https://github.com/mixxxdj/mixxx/pull/2555) [#2543](https://github.com/mixxxdj/mixxx/pull/2543) [#2779](https://github.com/mixxxdj/mixxx/pull/2779)
  * Note: The first Rekordbox memory cue is imported for the main cue button in Mixxx and the remaining Rekordbox memory cues are imported as Mixxx hotcues, starting with the next hotcue number after the last hotcue from Rekordbox.
  * Note: Mixxx does not yet support multiple loops per track. Imported loops from Rekordbox are treated like imported loops from Serato, so refer to the note above for details.

### Intro & Outro Cues

* Add intro & outro range cues with automatic silence detection [#1242](https://github.com/mixxxdj/mixxx/pull/1242)
* Show duration of intro & outro ranges on overview waveform [#2089](https://github.com/mixxxdj/mixxx/pull/2089)
* Use intro & outro cues in AutoDJ transitions [#2103](https://github.com/mixxxdj/mixxx/pull/2103)

### Deck cloning

* Add deck cloning (also known as "instant doubles" in other DJ software) by dragging and dropping between decks [#1892](https://github.com/mixxxdj/mixxx/pull/1892) and samplers [#3200](https://github.com/mixxxdj/mixxx/pull/3200)
* Clone decks by double pressing the load button on a controller (with option to disable this) [#2024](https://github.com/mixxxdj/mixxx/pull/2024) [#2042](https://github.com/mixxxdj/mixxx/pull/2042)

### Skins & GUI

* Aesthetically revamped LateNight skin [#2298](https://github.com/mixxxdj/mixxx/pull/2298) [#2342](https://github.com/mixxxdj/mixxx/pull/2342)
* Right click overview waveform to show time remaining until that point [#2238](https://github.com/mixxxdj/mixxx/pull/2238)
* Show track info dialog when double clicking track labels in decks [#2990](https://github.com/mixxxdj/mixxx/pull/2990)
* Show track context menu when right clicking text in decks [#2612](https://github.com/mixxxdj/mixxx/pull/2612) [#2675](https://github.com/mixxxdj/mixxx/pull/2675) [#2684](https://github.com/mixxxdj/mixxx/pull/2684) [#2696](https://github.com/mixxxdj/mixxx/pull/2696)
* Add laptop battery widget to skins [#2283](https://github.com/mixxxdj/mixxx/pull/2283) [#2277](https://github.com/mixxxdj/mixxx/pull/2277) [#2250](https://github.com/mixxxdj/mixxx/pull/2250) [#2228](https://github.com/mixxxdj/mixxx/pull/2228) [#2221](https://github.com/mixxxdj/mixxx/pull/2221) [#2163](https://github.com/mixxxdj/mixxx/pull/2163) [#2160](https://github.com/mixxxdj/mixxx/pull/2160) [#2147](https://github.com/mixxxdj/mixxx/pull/2147) [#2281](https://github.com/mixxxdj/mixxx/pull/2281) [#2319](https://github.com/mixxxdj/mixxx/pull/2319) [#2287](https://github.com/mixxxdj/mixxx/pull/2287)
* Show when passthrough mode is active on overview waveforms [#2575](https://github.com/mixxxdj/mixxx/pull/2575) [#2616](https://github.com/mixxxdj/mixxx/pull/2616)
* Changed format of currently playing track in window title from "artist, title" to "artist - title" [#2807](https://github.com/mixxxdj/mixxx/pull/2807)
* Workaround Linux skin change crash [#3144](https://github.com/mixxxdj/mixxx/pull/3144) [lp:1885009](https://bugs.launchpad.net/mixxx/+bug/1885009)
* Fix touch control [lp:1895431](https://bugs.launchpad.net/mixxx/+bug/1895431)
* Fix broken knob interaction on touchscreens [#3512](https://github.com/mixxxdj/mixxx/pull/3512)
* AutoDJ: Make "enable" shortcut work after startup [#3242](https://github.com/mixxxdj/mixxx/pull/3242)
* Add rate range indicator [#3693](https://github.com/mixxxdj/mixxx/pull/3693)
* Allow menubar to be styled [#3372](https://github.com/mixxxdj/mixxx/pull/3372) [#3788](https://github.com/mixxxdj/mixxx/pull/3788)
* Add Donate button to About dialog [#3838](https://github.com/mixxxdj/mixxx/pull/3838) [#3846](https://github.com/mixxxdj/mixxx/pull/3846)
* Add Scrollable Skin Widget [#3890](https://github.com/mixxxdj/mixxx/pull/3890)
* Fix minor visual issues in Skins [#3958](https://github.com/mixxxdj/mixxx/pull/3958/) [#3954](https://github.com/mixxxdj/mixxx/pull/3954/) [#3941](https://github.com/mixxxdj/mixxx/pull/3941/) [#3938](https://github.com/mixxxdj/mixxx/pull/3938/) [#3936](https://github.com/mixxxdj/mixxx/pull/3936/) [#3886](https://github.com/mixxxdj/mixxx/pull/3886/) [#3927](https://github.com/mixxxdj/mixxx/pull/3927/) [#3844](https://github.com/mixxxdj/mixxx/pull/3844/) [#3933](https://github.com/mixxxdj/mixxx/pull/3933/) [#3835](https://github.com/mixxxdj/mixxx/pull/3835/) [#3902](https://github.com/mixxxdj/mixxx/pull/3902) [#3931](https://github.com/mixxxdj/mixxx/pull/3931)

### Music Feature Analysis

* Multithreaded analysis for much faster batch analysis on multicore CPUs [#1624](https://github.com/mixxxdj/mixxx/pull/1624) [#2142](https://github.com/mixxxdj/mixxx/pull/2142) [lp:1641153](https://bugs.launchpad.net/mixxx/+bug/1641153)
* Fix bugs affecting key detection accuracy [#2137](https://github.com/mixxxdj/mixxx/pull/2137) [#2152](https://github.com/mixxxdj/mixxx/pull/2152) [#2112](https://github.com/mixxxdj/mixxx/pull/2112) [#2136](https://github.com/mixxxdj/mixxx/pull/2136)
  * Note: Users who have not manually corrected keys are advised to clear all keys in their library by pressing Ctrl + A in the library, right clicking, going to Reset -> Key, then reanalyzing their library. This will freeze the GUI while Mixxx clears the keys; this is a known problem that we will not be able to fix for 2.3. Wait until it is finished and you will be able to reanalyze tracks for better key detection results.
* Remove VAMP plugin support and use Queen Mary DSP library directly. vamp-plugin-sdk and vamp-hostsdk are no longer required dependencies. [#926](https://github.com/mixxxdj/mixxx/pull/926)
* Improvements BPM detection on non-const beatgrids [#3626](https://github.com/mixxxdj/mixxx/pull/3626)
* Fix const beatgrid placement [#3965](https://github.com/mixxxdj/mixxx/pull/3965) [#3973](https://github.com/mixxxdj/mixxx/pull/3973)

### Music Library

* Add support for searching for empty fields (for example `crate:""`) [lp:1788086](https://bugs.launchpad.net/mixxx/+bug/1788086)
* Improve synchronization of track metadata and file tags [#2406](https://github.com/mixxxdj/mixxx/pull/2406)
* Library Scanner: Improve hashing of directory contents [#2497](https://github.com/mixxxdj/mixxx/pull/2497)
* Rework of Cover Image Hashing [lp:1607097](https://bugs.launchpad.net/mixxx/+bug/1607097) [#2507](https://github.com/mixxxdj/mixxx/pull/2507) [#2508](https://github.com/mixxxdj/mixxx/pull/2508)
* MusicBrainz: Handle 301 status response [#2510](https://github.com/mixxxdj/mixxx/pull/2510)
* MusicBrainz: Add extended metadata support [lp:1581256](https://bugs.launchpad.net/mixxx/+bug/1581256) [#2522](https://github.com/mixxxdj/mixxx/pull/2522)
* TagLib: Fix detection of empty or missing file tags [lp:1865957](https://bugs.launchpad.net/mixxx/+bug/1865957) [#2535](https://github.com/mixxxdj/mixxx/pull/2535)
* Fix caching of duplicate tracks that reference the same file [#3027](https://github.com/mixxxdj/mixxx/pull/3027)
* Use 6 instead of only 4 compatible musical keys (major/minor) [#3205](https://github.com/mixxxdj/mixxx/pull/3205)
* Fix possible crash when trying to refocus the tracks table while another Mixxx window has focus [#3201](https://github.com/mixxxdj/mixxx/pull/3201)
* Don't create new tags in file when exporting metadata to it [#3898](https://github.com/mixxxdj/mixxx/pull/3898)
* Fix playlist files beginning with non-english characters not being loaded [#3916](https://github.com/mixxxdj/mixxx/pull/3916)
* Enable sorting in "Hidden Tracks" and "Missing Tracks" views [#3828](https://github.com/mixxxdj/mixxx/pull/3828) [lp:1828555](https://bugs.launchpad.net/mixxx/+bug/1828555/) [lp:1924616](https://bugs.launchpad.net/mixxx/+bug/1924616/)
* Fix track table being empty after start [#3935](https://github.com/mixxxdj/mixxx/pull/3935/) [lp:1930546](https://bugs.launchpad.net/mixxx/+bug/1930546/) [lp:1924843](https://bugs.launchpad.net/mixxx/+bug/1924843/)

### Audio Codecs

* Add FFmpeg audio decoder, bringing support for ALAC files [#1356](https://github.com/mixxxdj/mixxx/pull/1356)
* Include LAME MP3 encoder with Mixxx now that the MP3 patent has expired [lp:1294128](https://bugs.launchpad.net/mixxx/+bug/1294128) [buildserver:#37](https://github.com/mixxxdj/buildserver/pull/37) [buildserver:9e8bcee](https://github.com/mixxxdj/buildserver/commit/9e8bcee771731920ae82f3e076d43f0fb51e5027)
* Add Opus streaming and recording support. [lp:1338413](https://bugs.launchpad.net/mixxx/+bug/1338413)
* Remove support for SoundSource plugins because the code was not well-maintained and could lead to crashes [lp:1792747](https://bugs.launchpad.net/mixxx/+bug/1792747)
* Add HE-AAC encoding capabilities for recording and broadcasting [#3615](https://github.com/mixxxdj/mixxx/pull/3615)

### Audio Engine

* Fix loss of precision when dealing with floating-point sample positions while setting loop out position and seeking using vinyl control [#3126](https://github.com/mixxxdj/mixxx/pull/3126) [#3127](https://github.com/mixxxdj/mixxx/pull/3127)
* Prevent moving a loop beyond track end [#3117](https://github.com/mixxxdj/mixxx/pull/3117) [lp:1799574](https://bugs.launchpad.net/mixxx/+bug/1799574)
* Fix possible memory corruption using JACK on Linux [#3160](https://github.com/mixxxdj/mixxx/pull/3160)
* Fix changing of vinyl lead-in time [lp:1915483](https://bugs.launchpad.net/mixxx/+bug/1915483) [#3781](https://github.com/mixxxdj/mixxx/pull/3781)
* Fix tempo change of non-const beatgrid track on audible deck when cueing another track [#3772](https://github.com/mixxxdj/mixxx/pull/3772)
* Fix crash when changing effect unit routing [#3882](https://github.com/mixxxdj/mixxx/pull/3882) [lp:1775497](https://bugs.launchpad.net/mixxx/+bug/1775497)
* Make microphone ducking use strength knob the same way in automatic & manual mode [#2750](https://github.com/mixxxdj/mixxx/pull/2750)

### Controllers

* Improve workflow for configuring controller mappings and editing mappings [#2569](https://github.com/mixxxdj/mixxx/pull/2569) [#3278](https://github.com/mixxxdj/mixxx/pull/3278) [#3667](https://github.com/mixxxdj/mixxx/pull/3667)
* Improve error reporting from controller scripts [#2588](https://github.com/mixxxdj/mixxx/pull/2588)
* Make hotcue and track colors mappable on controllers [#2030](https://github.com/mixxxdj/mixxx/pull/2030) [#2541](https://github.com/mixxxdj/mixxx/pull/2541) [#2665](https://github.com/mixxxdj/mixxx/pull/2665) [#2520](https://github.com/mixxxdj/mixxx/pull/2520)
* Add way to change library table sorting from controllers [#2118](https://github.com/mixxxdj/mixxx/pull/2118)
* Add support for velocity sensitive sampler buttons in Components JS library [#2032](https://github.com/mixxxdj/mixxx/pull/2032)
* Add logging when script ControlObject callback is disconnected successfully [#2054](https://github.com/mixxxdj/mixxx/pull/2054)
* Add controller mapping for Roland DJ-505 [#2111](https://github.com/mixxxdj/mixxx/pull/2111)
* Add controller mapping for Numark iDJ Live II [#2818](https://github.com/mixxxdj/mixxx/pull/2818)
* Add controller mapping for Hercules DJControl Inpulse 200 [#2542](https://github.com/mixxxdj/mixxx/pull/2542)
* Add controller mapping for Hercules DJControl Jogvision [#2370](https://github.com/mixxxdj/mixxx/pull/2370)
* Add controller mapping for Pioneer DDJ-200 [#3185](https://github.com/mixxxdj/mixxx/pull/3185) [#3193](https://github.com/mixxxdj/mixxx/pull/3193) [#3742](https://github.com/mixxxdj/mixxx/pull/3742) [#3793](https://github.com/mixxxdj/mixxx/pull/3793) [#3949](https://github.com/mixxxdj/mixxx/pull/3949)
* Add controller mapping for Pioneer DDJ-400 [#3479](https://github.com/mixxxdj/mixxx/pull/3479)
* Add controller mapping for ION Discover DJ Pro [#2893](https://github.com/mixxxdj/mixxx/pull/2893)
* Add controller mapping for Native Instrument Traktor Kontrol S3 [#3031](https://github.com/mixxxdj/mixxx/pull/3031)
* Add controller mapping for Behringer BCR2000 [#3342](https://github.com/mixxxdj/mixxx/pull/3342) [#3943](https://github.com/mixxxdj/mixxx/pull/3943)
* Add controller mapping for Behringer DDM4000 [#3542](https://github.com/mixxxdj/mixxx/pull/3542)
* Update controller mapping for Allen & Heath Xone K2 to add intro/outro cues [#2236](https://github.com/mixxxdj/mixxx/pull/2236)
* Update controller mapping for Hercules P32 for more accurate headmix control [#3537](https://github.com/mixxxdj/mixxx/pull/3537)
* Update controller mapping for Native Instruments Traktor Kontrol S4MK2 to add auto-slip mode and pitch fader range [#3331](https://github.com/mixxxdj/mixxx/pull/3331)
* Fix Pioneer DDJ-SB2 controller mapping auto tempo going to infinity bug [#2559](https://github.com/mixxxdj/mixxx/pull/2559) [lp:1846403](https://bugs.launchpad.net/mixxx/+bug/1846403)
* Fix Numark Mixtrack Pro 3 controller mapping inverted FX on/off control [#3758](https://github.com/mixxxdj/mixxx/pull/3758)
* Gracefully handle MIDI overflow [#825](https://github.com/mixxxdj/mixxx/pull/825)

### Other

* Add CMake build system with `ccache` and `sccache` support for faster compilation times and remove SCons [#2280](https://github.com/mixxxdj/mixxx/pull/2280) [#3618](https://github.com/mixxxdj/mixxx/pull/3618)
* Make Mixxx compile even though `QT_NO_OPENGL` or `QT_OPENGL_ES_2` is defined (fixes build on Raspberry Pi) [lp:1863440](https://bugs.launchpad.net/mixxx/+bug/1863440) [#2504](https://github.com/mixxxdj/mixxx/pull/2504)
* Fix ARM build issues [#3602](https://github.com/mixxxdj/mixxx/pull/3602)
* Fix missing manual in DEB package [lp:1889776](https://bugs.launchpad.net/mixxx/+bug/1889776) [#2985](https://github.com/mixxxdj/mixxx/pull/2985)
* Add macOS codesigning and notarization to fix startup warnings [#3281](https://github.com/mixxxdj/mixxx/pull/3281)
* Don't trash user configuration if an error occurs when writing [#3192](https://github.com/mixxxdj/mixxx/pull/3192)
* Enable CUE sheet recording by default [#3374](https://github.com/mixxxdj/mixxx/pull/3374)
* Fix crash when double clicking GLSL waveforms with right mouse button [#3904](https://github.com/mixxxdj/mixxx/pull/3904)
* Derive Mixxx version from `git describe` [#3824](https://github.com/mixxxdj/mixxx/pull/3824) [#3841](https://github.com/mixxxdj/mixxx/pull/3841) [#3848](https://github.com/mixxxdj/mixxx/pull/3848)
* Improve tapping the bpm of a deck [#3790](https://github.com/mixxxdj/mixxx/pull/3790) [lp:1882776](https://bugs.launchpad.net/mixxx/+bug/1882776)
* And countless other small fixes and improvements (too many to list them all!)

## [2.2.4](https://launchpad.net/mixxx/+milestone/2.2.4) (2020-06-27)

* Store default recording format after "Restore Defaults" lp:1857806 #2414
* Prevent infinite loop when decoding corrupt MP3 files #2417
* Add workaround for broken libshout versions #2040 #2438
* Speed up purging of tracks lp:1845837 #2393
* Don't stop playback if vinyl passthrough input is configured and PASS button is pressed #2474
* Fix debug assertion for invalid crate names lp:1861431 #2477
* Fix crashes when executing actions on tracks that already disappeared from the DB #2527
* AutoDJ: Skip next track when both deck are playing lp:1399974 #2531
* Tweak scratch parameters for Mixtrack Platinum #2028
* Fix auto tempo going to infinity on Pioneer DDJ-SB2 #2559
* Fix bpm.tapButton logic and reject missed & double taps #2594
* Add controller mapping for Native Instruments Traktor Kontrol S2 MK3 #2348
* Add controller mapping for Soundless joyMIDI #2425
* Add controller mapping for Hercules DJControl Inpulse 300 #2465
* Add controller mapping for Denon MC7000 #2546
* Add controller mapping for Stanton DJC.4 #2607
* Fix broadcasting via broadcast/recording input lp:1876222 #2743
* Only apply ducking gain in manual ducking mode when talkover is enabed lp:1394968 lp:1737113 lp:1662536 #2759
* Ignore MIDI Clock Messages (0xF8) because they are not usable in Mixxx and inhibited the screensaver #2786

## [2.2.3](https://launchpad.net/mixxx/+milestone/2.2.3) (2019-11-24)

* Don't make users reconfigure sound hardware when it has not changed #2253
* Fix MusicBrainz metadata lookup [lp:1848887](https://bugs.launchpad.net/mixxx/+bug/1848887) #2328
* Fix high DPI scaling of cover art #2247
* Fix high DPI scaling of cue point labels on scrolling waveforms #2331
* Fix high DPI scaling of sliders in Tango skin #2318
* Fix sound dropping out during recording [lp:1842679](https://bugs.launchpad.net/mixxx/+bug/1842679) #2265 #2305 #2308 #2309
* Fix rare crash on application shutdown #2293
* Workaround various rare bugs caused by database inconsistencies [lp:1846971](https://bugs.launchpad.net/mixxx/+bug/1846971) #2321
* Improve handling of corrupt FLAC files #2315
* Don't immediately jump to loop start when loop_out is pressed in quantized mode [lp:1837077](https://bugs.launchpad.net/mixxx/+bug/1837077) #2269
* Preserve order of tracks when dragging and dropping from AutoDJ to playlist [lp:1829601](https://bugs.launchpad.net/mixxx/+bug/1829601) #2237
* Explicitly use X11 Qt platform plugin instead of Wayland in .desktop launcher [lp:1850729](https://bugs.launchpad.net/mixxx/+bug/1850729) #2340
* Pioneer DDJ-SX: fix delayed sending of MIDI messages with low audio buffer sizes #2326
* Enable modplug support on Linux by default [lp:1840537](https://bugs.launchpad.net/mixxx/+bug/1840537) #2244 #2272
* Fix keyboard shortcut for View > Skin Preferences [lp:1851993](https://bugs.launchpad.net/mixxx/+bug/1851993) #2358 #2372
* Reloop Terminal Mix: Fix mapping of sampler buttons 5-8 [lp:1846966](https://bugs.launchpad.net/mixxx/+bug/1846966) #2330

## [2.2.2](https://launchpad.net/mixxx/+milestone/2.2.2) (2019-08-10)

* Fix battery widget with upower <= 0.99.7. #2221
* Fix BPM adjust in BpmControl. lp:1836480
* Disable track metadata export for .ogg files and TagLib 1.11.1. lp:1833190
* Fix interaction of hot cue buttons and looping. lp:1778246
* Fix detection of moved tracks. #2197
* Fix playlist import. lp:16878282
* Fix updating playlist labels. lp:1837315
* Fix potential segfault on exit. lp:1828360
* Fix parsing of invalid bpm values in MP3 files. lp:1832325
* Fix crash when removing rows from empty model. #2128
* Fix high DPI scaling of RGB overview waveforms. #2090
* Fix for OpenGL SL detection on macOS. lp:1828019
* Fix OpenGL ES detection. lp:1825461
* Fix FX1/2 buttons missing Mic unit in Deere (64 samplers). lp:1837716
* Tango64: Re-enable 64 samplers. #2223
* Numark DJ2Go re-enable note-off for deck A cue button. #2087
* Replace Flanger with QuickEffect in keyboard mapping. #2233

## [2.2.1](https://launchpad.net/mixxx/+milestone/2.2.1) (2019-04-22)

* Include all fixes from Mixxx 2.1.7 and 2.1.8
* Fix high CPU usage on MAC due to preview column lp:1812763
* Fix HID controller output on Windows with common-hid-packet-parser.js
* Fix rendering slow down by not using QStylePainter in WSpinny lp:1530720
* Fix broken Mic mute button lp:1782568
* added quick effect enable button to the control picker menu
* Fix Cover Window close issue with empty cover arts
* Fix Numark Mixtrack 3 mapping. #2057

## [2.2.0](https://launchpad.net/mixxx/+milestone/2.2.0) (2018-12-17)

### General

* Update from Qt4 to Qt5.
* Use Qt5's automatic high DPI scaling (and remove the old
  scaling option from the preferences).
* Vectorize remaining raster graphics for better HiDPI support.

### Effects

* Add mix mode switch (Dry/Wet vs Dry+Wet) for effect units.
* Add support for LV2 effects plugins (currently no way to show plugin GUIs).
* Add preference option for selecting which effects are shown in the
  list of available effects in the main window (all LV2 effects are
  hidden by default and must be explicitly enabled by users).

### Skins

* Add 8 sampler and small sampler options to LateNight.
* Add key / BPM expansion indicators to Deere decks.
* Add skin settings menu to LateNight.

### Controllers

* Add controller mapping for Numark Mixtrack Platinum.
* Update controller mapping for Numark N4.
* Add spinback and break for Vestax VCI-400 mapping.

### Miscellaneous

* Add preference option to adjust the play position marker of
  scrolling waveforms.
* Add preference option to adjust opacity of beatgrid markers on
  scrolling waveforms.
* Support IRC/AIM/ICQ broadcast metadata.

## [2.1.8](https://launchpad.net/mixxx/+milestone/2.1.8) (2019-04-07)

* Fix a rare chance for a corrupt track file while writing metadata in out of disk situations. lp:1815305
* Fix export of BPM track file metadata. lp:1816490
* Fix export of BPM track file metadata. lp:1816490
* Fix sending of broadcast metadata with TLS enabled libshout 2.4.1. lp:1817395
* Fix resdicovering purged tracks in all cases. lp:1821514
* Fix dropping track from OSX Finder. lp:1822424

## [2.1.7](https://launchpad.net/mixxx/+milestone/2.1.7) (2019-01-15)

* Fix syncing to doublespeed [lp:1808697](https://bugs.launchpad.net/mixxx/+bug/1808697)
* Fix issues when changing beats of a synced track [lp:1808698](https://bugs.launchpad.net/mixxx/+bug/1808698)
* Fix direction of pitch bend buttons when inverting rate slider [lp:1770745](https://bugs.launchpad.net/mixxx/+bug/1770745)
* Use first loaded deck if no playing deck is found [lp:1784185](https://bugs.launchpad.net/mixxx/+bug/1784185)
* Encode file names correctly on macOS [lp:1776949](https://bugs.launchpad.net/mixxx/+bug/1776949)

## [2.1.6](https://launchpad.net/mixxx/+milestone/2.1.6) (2018-12-23)

* Fix crash when loading a Qt5 Soundsource / Vamp Plug-In. [lp:1774639](https://bugs.launchpad.net/mixxx/+bug/1774639)
* Validate effect parameter range. [lp:1795234](https://bugs.launchpad.net/mixxx/+bug/1795234)
* Fix crash using the bpm_tap button without a track loaded. [lp:1801844](https://bugs.launchpad.net/mixxx/+bug/1801844)
* Fix possible crash after ejecting a track. [lp:1801874](https://bugs.launchpad.net/mixxx/+bug/1801874)
* Fix wrong bitrate reported for faulty mp3 files. [lp:1782912](https://bugs.launchpad.net/mixxx/+bug/1782912)
* Fix Echo effect syncing [lp:1793232](https://bugs.launchpad.net/mixxx/+bug/1793232)
* Fix iTunes context menu [lp:1799932](https://bugs.launchpad.net/mixxx/+bug/1799932)
* Fix loading the wrong track after delete search and scroll. [lp:1803148](https://bugs.launchpad.net/mixxx/+bug/1803148)
* Improve search bar timing. [lp:1635087](https://bugs.launchpad.net/mixxx/+bug/1635087)
* Fix quoted search sentence. [lp:1784141](https://bugs.launchpad.net/mixxx/+bug/1784141)
* Fix loading a track formerly not existing. [lp:1800395](https://bugs.launchpad.net/mixxx/+bug/1800395)
* Fix importing m3u files with blank lines. [lp:1806271](https://bugs.launchpad.net/mixxx/+bug/1806271)
* Fix position in sampler overview waveforms. [lp:1744170](https://bugs.launchpad.net/mixxx/+bug/1744170)
* Don't reset rate slider, syncing a track without a beatgrid. [lp:1783020](https://bugs.launchpad.net/mixxx/+bug/1783020)
* Clean up iTunes track context menu. [lp:1800335](https://bugs.launchpad.net/mixxx/+bug/1800335)
* Collapsed sampler are not analyzed on startup. [lp:1801126](https://bugs.launchpad.net/mixxx/+bug/1801126)
* search for decoration characters like "˚". [lp:1802730](https://bugs.launchpad.net/mixxx/+bug/1802730)
* Fix cue button blinking after pressing eject on an empty deck. [lp:1808222](https://bugs.launchpad.net/mixxx/+bug/1808222)

## [2.1.5](https://launchpad.net/mixxx/+milestone/2.1.5) (2018-10-28)

* Code signing for Windows builds. [lp:1517823](https://bugs.launchpad.net/mixxx/+bug/1517823)
* Fix crash on exit when preferences is open. [lp:1793185](https://bugs.launchpad.net/mixxx/+bug/1793185)
* Fix crash when analyzing corrupt MP3s. [lp:1793387](https://bugs.launchpad.net/mixxx/+bug/1793387)
* Fix crash when importing metadata from MusicBrainz. [lp:1794993](https://bugs.launchpad.net/mixxx/+bug/1794993)
* Library search fixes when single quotes are used. [lp:1784090](https://bugs.launchpad.net/mixxx/+bug/1784090) [lp:1789728](https://bugs.launchpad.net/mixxx/+bug/1789728)
* Fix scrolling waveform on Windows with WDM-KS sound API. [lp:1729345](https://bugs.launchpad.net/mixxx/+bug/1729345)
* Fix right clicking on beatgrid alignment button in Tango and LateNight skins. [lp:1798237](https://bugs.launchpad.net/mixxx/+bug/1798237)
* Improve speed of importing iTunes library. [lp:1785545](https://bugs.launchpad.net/mixxx/+bug/1785545)
* Add 2 deck mapping for DJTechTools MIDI Fighter Twister.

## [2.1.4](https://launchpad.net/mixxx/+milestone/2.1.4) (2018-08-29)

Fix track selection not getting shown in the track
table on Windows. There are no changes to the
source code, but the Jenkins build configuration
was changed to delete the Jenkins workspace before
each build. [lp:1751482](https://bugs.launchpad.net/mixxx/+bug/1751482)

## [2.1.3](https://launchpad.net/mixxx/+milestone/2.1.3) (2018-08-20)

Fix a severe performance regression on Windows:
[Mixxx 2.1.2 running much slower than 2.1.1](https://mixxx.org/forums/viewtopic.php?f=3&t=12082)

## [2.1.2](https://launchpad.net/mixxx/+milestone/2.1.2) (2018-08-10)

Yet another bugfix release of Mixxx 2.1.
Here is a quick summary of what is new in Mixxx 2.1.2:

* Allow maximum deck speed of 4x normal.
* Don't always quantize hotcues, a 2.1.1 regression. [lp:1777429](https://bugs.launchpad.net/mixxx/+bug/1777429)
* Fix artifacts using more than 32 samplers. [lp:1779559](https://bugs.launchpad.net/mixxx/+bug/1779559)
* store No EQ and Filter persistently. [lp:1780479](https://bugs.launchpad.net/mixxx/+bug/1780479)
* Pad unreadable samples with silence on cache miss. [lp:1777480](https://bugs.launchpad.net/mixxx/+bug/1777480)
* Fixing painting of preview column for Qt5 builds. [lp:1776555](https://bugs.launchpad.net/mixxx/+bug/1776555)
* LateNight: Fix play button right click. [lp:1781829](https://bugs.launchpad.net/mixxx/+bug/1781829)
* LateNight: Added missing sort up/down buttons.
* Fix sampler play button tooltips. [lp:1779468](https://bugs.launchpad.net/mixxx/+bug/1779468)
* Shade: remove superfluid margins and padding in sampler.xml. [lp:1773588](https://bugs.launchpad.net/mixxx/+bug/1773588)
* Deere: Fix background-color code.
* ITunes: Don't stop import in case of duplicated Playlists. [lp:1783493](https://bugs.launchpad.net/mixxx/+bug/1783493)

## [2.1.1](https://launchpad.net/mixxx/+milestone/2.1.1) (2018-06-13)

After two months it is time to do a bugfix release of Mixxx 2.1.
Here is a quick summary of what is new in Mixxx 2.1.1:

* Require Soundtouch 2.0 to avoid segfault. [lp:1577042](https://bugs.launchpad.net/mixxx/+bug/1577042)
* Improved skins including library view fix. [lp:1773709](https://bugs.launchpad.net/mixxx/+bug/1773709) [lp:1772202](https://bugs.launchpad.net/mixxx/+bug/1772202) [lp:1763953](https://bugs.launchpad.net/mixxx/+bug/1763953)
* Fix crash when importing ID3v2 APIC frames. [lp:1774790](https://bugs.launchpad.net/mixxx/+bug/1774790)
* Synchronize execution of Vamp analyzers. [lp:1743256](https://bugs.launchpad.net/mixxx/+bug/1743256)
* DlgTrackInfo: Mismatching signal/slot connection.
* Detect M4A decoding errors on Windows. [lp:1766834](https://bugs.launchpad.net/mixxx/+bug/1766834)
* Fix spinback inertia effect.
* Fix decoding fixes and upgrade DB schema. [lp:1766042](https://bugs.launchpad.net/mixxx/+bug/1766042) [lp:1769717](https://bugs.launchpad.net/mixxx/+bug/1769717)
* Fix integration of external track libraries. [lp:1766360](https://bugs.launchpad.net/mixxx/+bug/1766360)
* Fix memory leak when loading cover art. [lp:1767068](https://bugs.launchpad.net/mixxx/+bug/1767068)
* Fix clearing of ReplayGain gain/ratio in file tags. [lp:1766094](https://bugs.launchpad.net/mixxx/+bug/1766094)
* Fix crash when removing a quick link. [lp:1510068](https://bugs.launchpad.net/mixxx/+bug/1510068)
* Fidlib: Thread-safe and reentrant generation of filters. [lp:1765210](https://bugs.launchpad.net/mixxx/+bug/1765210)
* Fix unresponsive scrolling through crates & playlists using encoder. [lp:1719474](https://bugs.launchpad.net/mixxx/+bug/1719474)
* Swap default values for temp/perm rate changes. [lp:1764254](https://bugs.launchpad.net/mixxx/+bug/1764254)

## [2.1.0](https://launchpad.net/mixxx/+milestone/2.1.0) (2018-04-15)

After two years of hard work, we are pleased to announce Mixxx 2.1. We
have overhauled the effects system, redesigned the skins, added and improved
lots of controller mappings, rewrote the audio file decoders twice, and of
course fixed a bunch of bugs. Download it!

Here is a quick summary of what is new in Mixxx 2.1.0:

* Graphical interface scales for high resolution screens
* Overhauled Deere and LateNight skins
* New Tango skin
* Effects are synchronized to the tempo
* Effects are processed post-fader and post-crossfader and can be previewed in headphones
* One metaknob per effect with customizable parameter control for intuitive use of effect chains
* Nine new effects: Autopan, Biquad Equalizer, Biquad Full Kill Equalizer, Loudness Contour, Metronome, Parametric Equalizer, Phaser, Stereo Balance, Tremolo
* Loaded effects and their parameters are saved and restored when Mixxx restarts
* More transparent sounding equalizers (Biquad Equalizer and Biquad Full Kill Equalizer)
* Improved scratching sounds with jog wheels, vinyl control, and dragging waveforms with the mouse
* Simplified looping and beatjump controls
* Configurable rows of 8 samplers with up to 8 rows available for a total of 64 samplers
* Files loaded to samplers are reloaded when Mixxx restarts
* Improved volume normalization algorithm (EBU-R 128)
* Filter library table by crates
* Sort musical keys in library table by circle of fifths
* Write metadata tags back to audio files
* New JavaScript library for controller mapping
* Configure multiple Internet broadcasting stations and use multiple stations at the same time
* Broadcast and record microphones with direct monitoring and latency compensation
* Broadcast and record from an external mixer
* Booth output with independent gain knob for using sound cards with 6 output channels without an external mixer
* Prevent screensaver from starting while Mixxx is running
* CUP (Cue And Play) cue button mode
* Time remaining and time elapsed now take into account the tempo fader
* Clicking cover art now shows it full size in a separate window
* and of course, lots and lots of bug fixes.

Here are controllers with mappings that have been added or updated since the 2.0
release. Mappings marked with an asterisk (*) have been updated for the new
effects interface:

* American Audio VMS2
* American Audio VMS4
* Allen & Heath Xone K2/K1*
* Behringer CMD Micro
* Behringer CMD MM1*
* Behringer CMD Studio 4a
* Denon MC4000*
* Denon MC6000 Mk2*
* FaderFox DJ2
* Hercules DJ Console 4-Mx*
* Hercules DJ Control MP3 LE / Glow
* Hercules DJ Control Compact
* Hercules P32*
* Ion Discover DJ
* Korg Nanokontrol 2
* Korg KAOSS DJ
* M-Audio Xponent
* Native Instruments Traktor Kontrol S4 Mk2*
* Novation Launchpad Mk1 & Mk2
* Novation Twitch
* Numark Mixtrack Pro 3 & Numark Mixtrack 3*
* Pioneer DDJ-SB2*
* Pioneer DDJ-SX*
* Reloop Beatmix 2
* Reloop Beatmix 4
* Reloop Digital Jockey 3 ME
* Reloop Terminal Mix 2
* Reloop Terminal Mix 4
* Vestax VCI-100 Mk2
* Vestax Typhoon

For users upgrading from older versions of Mixxx, we have a few important
announcements. First, if you are using Windows, you will have to uninstall any
old versions of Mixxx before you can install 2.1. How to uninstall Mixxx
varies on different versions of Windows:

* Windows Vista, 7, and 8: [Start > Control Panel > Programs > Uninstall a
  Program](https://support.microsoft.com/en-us/help/2601726)
* Windows 10: [Start > Control Panel > Programs > Programs And Features >
  look for Mixxx > Uninstall](https://support.microsoft.com/en-gb/help/4028054/windows-repair-or-remove-programs-in-windows-10)

If you are upgrading from an older version of Mixxx and have MP3 files in
your library, we have another important announcement. The good news is that we
fixed a bug where the waveforms and audio playback of MP3 files were
misaligned. The bad news is that we have no way of knowing which MP3 files were
affected or how much the offset was. That means that waveforms, beatgrids,
cues, and loops from older versions of Mixxx may be offset by an unknown amount
for any MP3 file. Only MP3 files were affected by this bug; other audio file
types are unaffected. You can either correct your beatgrids and cue points
manually for each track, or you can clear this information for all MP3s and
start fresh. Regardless, we recommend clearing the waveforms for all MP3
files. To clear these, type "location:mp3" into the library search bar, press
Control + A to select all tracks, right click, and select the information you
want to clear from the menu.

In the works for Mixxx 2.2, we have a big redesign of the library GUI. Along
with that will come saving & restoring search queries plus nested crates.
We are also planning on adding support for saving and loading custom effect
chain presets with the ability to import and export them to share online.

Want to help make Mixxx even more awesome? The biggest thing we need is more
people. You do not need to be a programmer to help out. Giving feedback on the
design of new features as they are being made is very valuable. Refer to the
Testing page on the wiki for more information on how to get involved with that.
Reporting bugs and telling us your ideas on the Launchpad bug tracker is a big
help too! We cannot fix problems we do not know about, so please let us know if
you find any issues with Mixxx. If you would like to help translate Mixxx into
another language, refer to the Internationalization wiki page.  Of course, more
programmers could always help. Read the Developer Documentation on the wiki for
tips on getting started contributing code to Mixxx.

We hope you have as much fun with Mixxx as we do!

For a full list of new features and bugfixes, check out the
[2.1.0 milestone on Launchpad](https://launchpad.net/mixxx/+milestone/2.1.0).

## [2.0.0](https://launchpad.net/mixxx/+milestone/2.0.0) (2015-12-31)

* 4 Decks with Master Sync
* New Effects Framework with 4 Effect Units and 5 Built-in Effects:
  * Flanger, Bit Crusher, Reverb, Echo, Filter
  * More to come!
* Configurable, Resizeable User Interface with 3 Brand New Skins
* Cover Art Display
* Music Key Detection and Shifting
* Vinyl Audio Pass-Through
* 4 Microphone inputs and 4 Auxiliary inputs
* MIDI Mapping GUI and Improved Learning Wizard
* MusicBrainz metadata fetching
* RGB Musical Waveforms
* Hundreds of Bug Fixes and Improvements
* New Pitch-Independent Algorithm for Better-Sounding Key-lock

For a full list of new features and bugfixes, check out the
[2.0.0 milestone on Launchpad](https://launchpad.net/mixxx/+milestone/2.0.0).
