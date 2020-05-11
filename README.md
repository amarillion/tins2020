# Fole & Raul go Flower Power!

After the insane adventures of 'Fole & Raul go Bananas', our heroes have now
found a more groovy lifestyle. Dressed as hippies, they listen to old records,
and strum their guitars a little while hanging out in a relaxed flower garden.

But their days of adventuring are not over. Some of the local plant & insect life
is not as chilled out as our heroes, and they start to attack! Fend them off
as long as you can while collecting power ups to upgrade your weapons.

## How to play

 How to Play
-----------------------------------------------------------------------

The goal of the game is to collect all bananas in each level, before your time
runs out. You can play in single player mode or with two players behind the
same computer in cooperative mode.

In the menu you can turn sound on/off, change the number of players and reconfigure
keys.

Default keys:

Player 1: Fole the cat
Left = Left
Right = Right
Up = Up
Down = Down
Attack = Right Ctrl

Player 2: Raul the monkey
Left = A
Right = D
Up = W
Down = S
Attack = Left Ctrl

You can reconfigure these keys from the game menu. 
(The alternate action key shown in the menu, doesn't actually
do anything in the game)

Do not touch the monsters, as they will hurt you and kill you after a number
of hits. You can kill monsters with your guitar, by pressing the attack key.
You have unlimited ammo. Killing monsters will gain you XP (although XP 
doesn't actually do anything in the game, and hiscores are not recorded)

You can move between rooms in the level through doors or teleporters 
(that look like blue round swirly pads).

If you manage to collect all bananas before the time runs out and before you both
die, you will start in the next level. The enemies will get a little tougher, 
but also the player who collected the most bananas will get extra hitpoints.

In the game, press ESC to pause. You will return to the menu, with the option
to resume the game. From the menu you can press ESC again to quit.

The dungeons get tougher and larger with every new level.
Each level has at least one hidden power-up, that upgrades health, time, or your weapon.

## Implementation of TINS2020 rules

Rules:

```
**** There will be 1 genre rule

genre rule #130
Theme: Flowers - There should be different flowers in the game, the more the better!
```

The game is set in a flowery garden. The characters are dressed 
as 'Flower Power' hippies. Some of the enemies are flowers too!

```
**** There will be 1 artistical rule

artistical rule #60
make fun of old-fashioned things.
```

We're taking a humorous look at the 'Flower Power' era of the 60ies and 70ies. 
Some of the collectibles in the game are old-fashioned devices that you would no longer use.

```
**** There will be 2 technical rules

technical rule #10
Your game must include procedural content. Bonus points if procedural generation is used to create the gameplay environment.
```

The dungeons are randomly generated. We used Kruskal's algorithm to generate a maze. 
It has keys and locked doors, but it is guaranteed always solvable.
Bonuses and bananas are placed so that you'll almost never find an empty dead end.

```
technical rule #68
some kind of special text scroller
```

In game messages are announced as rainbow-coloured, horizontal scrolling text.

```
**** There will be 1 bonus rule

bonus rule #9
Act of monkey - you may replace a single rule by a rule that has the opposite effect
```
We do not invoke the bonus rule.

## Credits

Amarillion: code
Max: graphics, character design 
SUPERDAZE: Music, sound effects

## Code re-use and resources

Keep in mind that we've re-used a lot of code while developing this game.

Most of the engine code (in the 'twist5' directory) comes from 'Happy Usagi no Yuki Fortress, KrampusHack 2019 edition'
The core game loop and some assets come from our 2006 speedhack entry 'Fole & Raul go bananas'

We've replaced nearly all assets from speedhack 2006, with a few exceptions:
- The banana
- The doors (locked door sprite was already created in 2006, but never used before now!)
- The teleporter pad
- Also some of the monster sounds were re-used

The sample Alien Siren was derived from:
http://soundbible.com/1753-Alien-Siren.html
License: Public Domain | Recorded by KevanGC

The Font 'Spicy Rice' was obtained from: 
https://www.1001freefonts.com/spicy-rice.font
License: Public Domain / GPL / OFL