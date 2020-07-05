# Bot Battles

[Bot Battles](https://github.com/Sandruski/bot-battles) is a 2D online multiplayer shooter video game about programming your own bot and battling against others. Two clients compete in an authoritative server.

<iframe src="https://player.vimeo.com/video/435450816" width="640" height="329" frameborder="0" allow="autoplay; fullscreen" allowfullscreen></iframe>
<br>
<iframe src="https://player.vimeo.com/video/435451334" width="640" height="329" frameborder="0" allow="autoplay; fullscreen" allowfullscreen></iframe>
<br>
<iframe src="https://player.vimeo.com/video/435451400" width="640" height="329" frameborder="0" allow="autoplay; fullscreen" allowfullscreen></iframe>
<br>

# How to play

## Code a bot

1. Go to data>scripts>bots.
2. Copy exampleBot.py or create a new .py file in the same folder.
3. Open it in Visual Studio.
4. Go to data>scripts>stubs.
5. Open botbattles.pyi in Visual Studio too.
6. Go to docs.
7. Open botAPI.md.
8. Code your bot.
9. Once you're done, save it.

## Create a map

1. Download [Tiled](https://www.mapeditor.org/) and install it.
2. Go to data>source.
3. Open backyard.tmx or house.tmx or create a new map in Tiled.
4. Edit your map.
5. Once you're done, export it as a .json in data>maps.

## Battle

1. On the client, write the IP and the port of the server, your name, and select the bot that you want to play with. The bot can be one of the example bots (easyBot or hardBot) or your own bot.
2. On the server, write its port, and select the map that you want to play in and the duration of the game. The map can be one of the example maps (backyard or house) or your own map.
3. Wait until the connection between the client and the server is established.
4. Watch the deathmatch.
5. Once it is over, go to the directory of the executable of the game.
6. Open the .log file to check the output of your bot.

Have fun!

# Reference

## Events

Method | Description
------------ | -------------
onSeenNewBot | It is called when the bot starts seeing another bot.
onSeenNewWeapon | It is called when the bot starts seeing a weapon and ammunition box.
onSeenNewHealth | It is called when the bot starts seeing a first aid box.
onSeenLostBot | It is called when the bot stops seeing another bot.
onSeenLostWeapon | It is called when the bot stops seeing a weapon and ammunition box.
onSeenLostHealth | It is called when the bot stops seeing a first aid box.
onWeaponPickedUp | It is called when the bot picks up a weapon and an ammunition box.
onHealthPickedUp | It is called when the bot picks up a first aid box.
onBulletHit | It is called when one of the bot's bullets hits another bot.
onBulletMiss | It is called when one of the bot's bullets hits a wall or the floor.
onReloaded | It is called when the bot reloads its primary weapon.
onHitByBullet | It is called when the bot is hit by another bot's bullet.
onHealed | It is called when the bot heals itself.
onHitWall | It is called when the bot collides with a wall.
onKill | It is called when the bot kills its opponent.
onDeath | It is called when the bot is killed by its opponent.

## Input Component

Attribute | Description
------------ | -------------
linearVelocityX | The desired linear velocity of the bot on the horizontal axis (x-axis).
linearVelocityY | The desired linear velocity of the bot on the vertical axis (y-axis).
angularVelocity | The desired angular velocity of the bot.

Method | Description
------------ | -------------
shootPrimaryWeapon | Fires a bullet with the primary weapon of the bot (if any) in the direction that the bot is facing. It only shoots if the primary weapon has currentAmmo.
shootSecondaryWeapon | Fires a bullet with the secondary weapon of the bot in the direction that the bot is facing.
reload | Reloads the primary weapon of the bot (if any). It consumes the necessary ammunition from an ammunition box (if any) from the bot's inventory. It only reloads if the primary weapon's currentAmmo is less than maxAmmo and the bot has ammoBoxAmmo.
heal | Heals the bot. It consumes all HP from a first aid box (if any) from the bot's inventory. It only heals if the bot has firstAidBoxHP.

## Transform Component

Attribute | Description
------------ | -------------
position | The position of the bot measured in pixels. (0,0) is at the top left of the battlefield.
rotation | The rotation of the bot measured in degrees. 0 is towards the right of the battlefield.
direction | The normalized vector of the rotation of the bot measured in pixels. (1,0) is towards the right of the battlefield.

## Rigidbody Component

Attribute | Description
------------ | -------------
linearVelocity | The linear velocity of the bot measured in pixels/second.
angularVelocity | The angular velocity of the bot measured in degrees/second.
maxLinearVelocity | The maximum value that the linear velocity of the bot can take.
maxAngularVelocity | The maximum value that the angular velocity of the bot can take.

## Collider Component

Attribute | Description
------------ | -------------
size | The dimensions of the collider of the bot measured in pixels.

## Sight Component

Attribute | Description
------------ | -------------
seenBotEntities | A list of the entities of the seen bots.
seenWeaponEntities | A list of the entities of the seen weapons and ammunition boxes.
seenHealthEntities | A list of the entities of the seen first aid boxes.

Method | Description
------------ | -------------
getSeenBotInfo | Returns a snapshot of the entity of a seen bot. The snapshot is of its transform, rigidbody, collider, weapon, health, and action components.
getSeenWeaponInfo | Returns a snapshot of the entity of a seen weapon and ammunition box. The snapshot is of its transform, collider, and weapon components.
getSeenHealthInfo | Returns a snapshot of the entity of a seen first aid box. The snapshot is of its transform, collider, and health components.

## Weapon Component

Attribute | Description
------------ | -------------
primaryWeaponDamage | The damage dealt by a bullet fired with the primary weapon.
secondaryWeaponDamage | The damage dealt by a bullet fired with the secondary weapon.
currentAmmo | The ammunition contained by the primary weapon.
maxAmmo | The maximum ammunition that the primary weapon can contain.
ammoBoxAmmo | The ammunition contained by the ammunition box used to reload the primary weapon.
primaryWeaponRange | The maximum distance (in pixels) that a bullet fired with the primary weapon can travel.
secondaryWeaponRange | The maximum distance (in pixels) that a bullet fired with the secondary weapon can travel.
primaryWeaponShootingTime | The seconds that it takes for the bot to shoot with the primary weapon.
primaryWeaponShootingCooldown | The seconds that the bot has to wait to perform another action after shooting with the primary weapon.
secondaryWeaponShootingTime | The seconds that it takes for the bot to shoot with the secondary weapon.
secondaryWeaponShootingCooldown | The seconds that the bot has to wait to perform another action after shooting with the secondary weapon
reloadingTime | The seconds that it takes for the bot to reload the primary weapon.
reloadingCooldown | The seconds that the bot has to wait to perform another action after reloading the primary weapon.

## Health Component

Attribute | Description
------------ | -------------
currentHP | The health points.
maxHP | The maximum health points.
firstAidBoxHP | The health points contained by the first aid box used to heal the bot.
healingTime | The seconds that it takes for the bot to heal itself.
healingCooldown | The seconds that the bot has to wait to perform another action after healing itself.

## Action Component

Attribute | Description
------------ | -------------
canPerformAction | Whether a bot is ready to perform a new action.

## Map Component

Attribute | Description
------------ | -------------
tileCount | The number of tiles in the map.

Method | Description
------------ | -------------
getTileType | Returns the type of a tile of the map. It can be a floor, a wall, a bot spawner, a weapon spawner, or a health spawner.
isVisible | Returns the visibility of a world position seen from another world position.
getWorldPosition | Returns the world position of a tile.
getMapPosition | Returns the tile of a world position.

## Collision

Attribute | Description
------------ | -------------
normal | The normal of the contact point measured in pixels.
relativeVelocity | The relative linear velocity of the two colliding objects measured in pixels/second.