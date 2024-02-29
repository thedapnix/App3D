#pragma once

//Each relic (upgrade) is 128 bits in memory and gets a unique ID through the use of GetId(), templates and static magic
struct DamageRelic
{
	char name[32] = "Strong Arms";
	char flavorText[96] = "Give big strong arms, 1.5x damage multiplier";
};

struct DefenseRelic
{
	char name[32] = "Big Boy";
	char flavorText[96] = "Am large, 0.5x damage taken";
};

struct SpeedRelic
{
	char name[32] = "Leg Day";
	char flavorText[96] = "Never skipped, 2x dash distance";
};