# Match idle

The idea of match idle is to create a match 3 idle game. 
The first goal is to create a decent match 3 game. 
Most other implementations I've found the match mechanic is severely lacking, it should be fast and responsive and you should at least get like cool things happening when you match 4 or 5 items together. 

I'm not yet sure what kind of idle system to couple with the match 3.  

Idle options:
 - We could have a resource gathering system where you build/upgrade buildings that help you out.
 - Some kind of RPG system where we defeat monsters
   (more in the like of Loop Hero or this unfinished game [matheld](https://www.reddit.com/r/incremental_games/comments/9qe3t2/pc_mathel_idle_an_idle_game_with_match_3_system/)).
   You collect artifacts that make the matching stronger and maybe somehow help you match stuff, or give you cool abilities to help with matching. Without a challenge it might not be too much fun. 
 - A very simple implementation where matching just gives you points [like this](https://www.kongregate.com/games/dlpl/idle-match-world). 
   This is quite an interesting game, it's just that the matching is not very responsive. I'd say this is less of an idle game and more of an incremental upgrade game.
 - Not match 3 but similar [Pocket Farmery](https://www.reddit.com/r/incremental_games/comments/6881l7/pocket_farmery_combination_of_match_3_and/), quite addictive. 
 Not sure if I like the prestige system much.

Currently I think the resource gathering might be fun. 


## Engine

There's quite a few options out there. 
There are full blown engines like unity, unreal engine and godot where you script more than program. 
I'd prefer to write my entire game in C++ and do everything in that.
So we're not per se using an engine but more a C/C++ graphics/sound/input library.
The library needs to be cross platform (so no DX9-11 and metal), so it can work on windows, IOS and android. 

With these requirements in mind there are only a few options left.
 - SDL. It is a c library that can target pretty much any platform.
 Even webassembly, it also is natively supported in Dear Imgui if we want to write quick interfaces to test/debug certain parts of the game.
 - SFML. It is a C++ library.
 Which at the moment doesn't support webassembly or Dear Imgui.

Since there are C++ wrappers around SDL I think I might go with that since it's slightly better supported and has multiple backends.
The best looking C++ wrapper: [centurion](https://github.com/albin-johansson/centurion).

## Art

We can get free game assets from the following sites:
 - [open game art](https://opengameart.org/)
 - [game icons](https://game-icons.net/tags/catan.html), found icons that could be useful for resource gathering

## Match 3 mechanics

I've been playing bejeweled 3 and slowed it down using cheat engine. I've found that you can match any gems when they're not moving. Falling gems cannot be moved and matches are determined at the start of the move animation, not the end. Also matching just before the rows start falling keeps those two gems in place and holds up any gems above it.

I probably need to find a better word then gems for my game since I won't be using gems. Maybe resources.

## TODO

- [x] Get C++ wrapper working
- [x] Render grid
- [ ] Load sprite
- [x] Fill board with items
- [x] Make items moveable by one
- [x] Recognize match 
- [ ] Add animation for swapping gems
- [ ] Add animation for dropping gems
- [ ] Recognize game over, no more possible combinations
- [ ] Allow matching while grid is still busy refilling and matching
- [ ] Add bomb when you have a match 4
- [ ] Add lightning which clears board when you have an L or T match
- [ ] Add clear color when you have a match 5
- [ ] Add clear board when you match two clear colors
- [ ] Add resource counters