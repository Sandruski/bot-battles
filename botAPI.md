# Bot API

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