# The Moonlight Tongue
 A Metroidvania game about...

 To Do:
- Currently, when the player lands on the edge of a rectangular platform, they'll sometimes get shunted off to the side (probably because the collision resolution system thinks that this is the best way to resolve a collision); it does this even when they're landing from straight above the object. We should remedy this by checking to see if the thing the player is landing on is a platform and, if so, trying to resolve the collision so that they end up on top of it.