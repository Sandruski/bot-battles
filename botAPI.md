# Bot API

## Bot events

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