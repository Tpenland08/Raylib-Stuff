#include "raylib.h"

typedef struct entity{
	Vector3 worldPos;
	float rotationRadians;
	Vector2 velo;
	float speed;
	float health;
	float maxHealth;
	int type; // 0:player, 1:docile, 2:enemy, 3:object
	bool visible;
} entity;

typedef struct weapon{
	bool unlocked;
	float damage;
	float cooldown;
	float fired;
	float bulletSpeed;
	int ammo;
} weapon;

typedef struct bullet{
    Vector2 worldPos;
    Vector2 velo;
    float damage;
    float speed;
    float lifetime;
} bullet;

typedef struct planet{
	Vector2 worldPos;
	float size;
	float gravity;
	Texture2D texture;
} planet;