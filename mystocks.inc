#if defined _sourcepawnstocks_included
#endinput
#endif
#define _sourcepawnstocks_included

#define MAX_MESSAGE_LENGTH 		250

#define MAX_TEAMS 				32
#define MAX_TEAM_NAME_LENGTH	32

#define MAX_WEAPON_OFFSET		64
#define MAX_WEAPON_SLOTS		6
#define MAX_WEAPON_POSITIONS	20
#define MAX_WEAPONS				48

#define WEAPON_NOCLIP			-1

#define	MAX_AMMO_TYPES			32
#define MAX_AMMO_SLOTS 			32

#define MAX_WEAPON_STING		80
#define MAX_WEAPON_PREFIX		16
#define MAX_WEAPON_AMMO_NAME	32

#define LoopClients(%1) for (int %1 = 1; %1 <= MaxClients; %1++) if (IsClientInGame(%1))
#define LoopClientsT(%1) for (int %1 = 1; %1 <= MaxClients; %1++) if (IsClientInGame(%1) && GetClientTeam(%1) == CS_TEAM_T)
#define LoopClientsCT(%1) for (int %1 = 1; %1 <= MaxClients; %1++) if (IsClientInGame(%1) && GetClientTeam(%1) == CS_TEAM_CT)
#define LoopClientsValid(%1) for (int %1 = 1; %1 <= MaxClients; %1++) if (IsValidClient(%1))
#define LoopClientsNoBots(%1) for (int %1 = 1; %1 <= MaxClients; %1++) if (IsClientInGame(%1) && !IsFakeClient(%1))
#define LoopClientsNoBotsT(%1) for (int %1 = 1; %1 <= MaxClients; %1++) if (IsClientInGame(%1) && !IsFakeClient(%1) && GetClientTeam(%1) == CS_TEAM_T)
#define LoopClientsNoBotsCT(%1) for (int %1 = 1; %1 <= MaxClients; %1++) if (IsClientInGame(%1) && !IsFakeClient(%1) && GetClientTeam(%1) == CS_TEAM_CT)
#define LoopClientsOnlyAlive(%1) for (int %1 = 1; %1 <= MaxClients; %1++) if (IsClientInGame(%1) && IsPlayerAlive(%1))
#define LoopClientsOnlyDead(%1) for (int %1 = 1; %1 <= MaxClients; %1++) if (IsClientInGame(%1) && !IsPlayerAlive(%1))

#include <sourcemod>
#include <sdktools>
#include <cstrike>
#include <sdkhooks>

bool IPAktif;

/*
** Kullanıcı valid mi diye kontrol eder
**
** Example:
** if (IsValidClient(client, true)
**/

stock bool IsValidClient(int client, bool nobots = true)
{
	if (client <= 0 || client > MaxClients || !IsClientConnected(client) || (nobots && IsFakeClient(client)))
	{
		return false;
	}
	return IsClientInGame(client);
}

/*
** Entity valid mi diye kontrol eder
**
** Example:
** if (IsValidEnt(ent))
**/

stock bool IsValidEnt(int ent)
{
	if (ent == INVALID_ENT_REFERENCE && ent <= MaxClients && !(IsValidEntity(ent) || IsValidEdict(ent)))
	{
		return false;
	}
	return true;
}

/*
** Kullanıcının belirlenen harfe erişimi var mı diye kontrol eder
**
** Example:
**	ConVar Test = null;
**	
**	public void OnPluginStart()
**	{
**	    Test = CreateConVar("sm_flag", "b", "Yetki Harfi");
**	}
**	char Flag[8];
**	Test.GetString(Flag, sizeof(Flag));
**	if CheckAdminFlag(client, Flag)
**/

stock bool CheckAdminFlag(int client, const char[] flags) // Z harfi otomatik erişim verir
{
	int iCount = 0;
	char sflagNeed[22][8], sflagFormat[64];
	bool bEntitled = false;
	Format(sflagFormat, sizeof(sflagFormat), flags);
	ReplaceString(sflagFormat, sizeof(sflagFormat), " ", "");
	iCount = ExplodeString(sflagFormat, ",", sflagNeed, sizeof(sflagNeed), sizeof(sflagNeed[]));
	for (int i = 0; i < iCount; i++)
	{
		if ((GetUserFlagBits(client) & ReadFlagString(sflagNeed[i])) || (GetUserFlagBits(client) & ADMFLAG_ROOT))
		{
			bEntitled = true;
			break;
		}
	}
	return bEntitled;
}

/*
** Kullanıcının belirlenen harfe erişimi var mı diye kontrol eder
**
** Example:
**	ConVar Test = null;
**	
**	public void OnPluginStart()
**	{
**	    Test = CreateConVar("sm_flag", "z", "Yetki Harfi");
**	}
**	char Flag[8];
**	Test.GetString(Flag, sizeof(Flag));
**	if CheckAdminFlagEx(client, Flag)
**/

stock bool CheckAdminFlagEx(int client, const char[] flags) // Z harfine otomatik erişim vermez
{
	int iCount = 0;
	char sflagNeed[22][8], sflagFormat[64];
	bool bEntitled = false;
	Format(sflagFormat, sizeof(sflagFormat), flags);
	ReplaceString(sflagFormat, sizeof(sflagFormat), " ", "");
	iCount = ExplodeString(sflagFormat, ",", sflagNeed, sizeof(sflagNeed), sizeof(sflagNeed[]));
	for (int i = 0; i < iCount; i++)
	{
		if ((GetUserFlagBits(client) & ReadFlagString(sflagNeed[i]) == ReadFlagString(sflagNeed[i])))
		{
			bEntitled = true;
			break;
		}
	}
	return bEntitled;
}

/* Admin olduğunu kontrol etme
**
** Example:
** if (IsPlayerGenericAdmin(client))
*/

stock bool IsPlayerGenericAdmin(int client)
{
	if ((GetAdminFlag(GetUserAdmin(client), Admin_Generic)))
	{
		return true;
	}
	return false;
}

/* Her hangi bir yetkiye sahip olup olmadığını kontrol etme
**
** Example:
** if (IsPlayerAnyAdmin(client))
*/

stock bool IsPlayerAnyAdmin(int client)
{
	if (GetUserAdmin(client) != INVALID_ADMIN_ID)
	{
		return true;
	}
	return false;
}

/* Oyuncu o bayrağa sahip mi diye kontrol eder
** 
** if (IsAdminFlags(client))
*/

stock bool IsAdminFlags(int client, int flags = ADMFLAG_GENERIC)
{
	AdminId adminId = GetUserAdmin(client);
	if (adminId == INVALID_ADMIN_ID) {
		return false;
	}
	return GetAdminFlags(adminId, Access_Effective) & flags == flags;
}

/*
** Gelişmiş bir şekilde silah vermeye yarar, verilen silahın şarjör ve mermisi ayarlanabilir
**
** Example:
** GivePlayerItemAmmo(client, "weapon_deagle", 1, 0);
**/

stock int GivePlayerItemAmmo(int client, const char[] weapon, int clip = -1, int ammo = -1)
{
	int weaponEnt = GivePlayerItem(client, weapon);
	SetPlayerWeaponAmmo(client, weaponEnt, clip, ammo);
	return weaponEnt;
}

stock int GivePlayerItemEx(int client, const char[] className)
{
	if (IsValidClient(client))
	{
		EquipPlayerWeapon(client, GivePlayerItem(client, className));
	}
	else
		return INVALID_ENT_REFERENCE;
}

stock void SetPlayerWeaponAmmo(int client, int weaponEnt, int clip = -1, int ammo = -1)
{
	if (weaponEnt == INVALID_ENT_REFERENCE || !IsValidEdict(weaponEnt))
		return;
	if (clip != -1)
		SetEntProp(weaponEnt, Prop_Data, "m_iClip1", clip);
	if (ammo != -1)
	{
		SetEntProp(weaponEnt, Prop_Send, "m_iPrimaryReserveAmmoCount", ammo);
		SetEntProp(weaponEnt, Prop_Send, "m_iSecondaryReserveAmmoCount", ammo);
	}
}

/*
** Kullanıcının silahlarını siler
**
** Example:
** ClearWeapon(client);
**/

stock void ClearWeapon(int client)
{
	for (int j = 0; j < 12; j++)
	{
		int weapon = GetPlayerWeaponSlot(client, j);
		if (weapon != -1)
		{
			RemovePlayerItem(client, weapon);
			RemoveEntity(weapon);
		}
	}
}

/*
** Kullanıcının silahlarını siler
**
** Example:
** ClearWeaponEx(client);
**/

stock void ClearWeaponEx(int client)
{
	int wepIdx;
	for (int i; i < 12; i++)
	{
		while ((wepIdx = GetPlayerWeaponSlot(client, i)) != -1)
		{
			RemovePlayerItem(client, wepIdx);
			RemoveEntity(wepIdx);
		}
	}
}

/*
** ConVar değişikliği yapar
**
** Example:
** SetCvar("mp_roundtime", 60);
**/

stock void SetCvar(char[] cvarName, int value)
{
	ConVar IntCvar = FindConVar(cvarName);
	if (IntCvar == null)return;
	int flags = IntCvar.Flags;
	flags &= ~FCVAR_NOTIFY;
	IntCvar.Flags = flags;
	IntCvar.IntValue = value;
	flags |= FCVAR_NOTIFY;
	IntCvar.Flags = flags;
}

/*
** ConVar değişikliği yapar
**
** Example:
** SetCvar("mp_roundtime", 60.0);
**/

stock void SetCvarFloat(char[] cvarName, float value)
{
	ConVar FloatCvar = FindConVar(cvarName);
	if (FloatCvar == null)return;
	int flags = FloatCvar.Flags;
	flags &= ~FCVAR_NOTIFY;
	FloatCvar.Flags = flags;
	FloatCvar.FloatValue = value;
	flags |= FCVAR_NOTIFY;
	FloatCvar.Flags = flags;
}

/*
** ConVar değişikliği yapar
**
** Example:
** SetCvar("mp_roundtime", "60");
**/

stock void SetCvarString(char[] cvarName, char[] value)
{
	ConVar StringCvar = FindConVar(cvarName);
	if (StringCvar == null)return;
	int flags = StringCvar.Flags;
	flags &= ~FCVAR_NOTIFY;
	StringCvar.Flags = flags;
	StringCvar.SetString(value);
	flags |= FCVAR_NOTIFY;
	StringCvar.Flags = flags;
}

/*
** Bir timer hızlı bir şekilde durdurmaya yarar
**
** Example:
** StopTimer(timer);
**/

stock void StopTimer(Handle &timer)
{
	if (timer != null)
	{
		delete timer;
		timer = null;
	}
}

/*
** Kullanıcının ekranını renklendirir
**
** Example:
** ScreenColor(client, Color);
**/

stock void ScreenColor(int client, int Color[4])
{
	int clients[1];
	clients[0] = client;
	Handle message = StartMessageEx(GetUserMessageId("Fade"), clients, 1, 0);
	Protobuf pb = UserMessageToProtobuf(message);
	pb.SetInt("duration", 200);
	pb.SetInt("hold_time", 40);
	pb.SetInt("flags", 17);
	pb.SetColor("clr", Color);
	EndMessage();
}

/*
** Kullanıcının açıkta duran menüsünü kapatır
**
** Example:
** CloseClientMenu(client);
**/

stock int CloseClientMenu(int client) // Oyuncuların mevcutta açık olan menüsünü iptal ettirmeye yarar.
{
	Menu Menu_CCM = new Menu(ClientMenu_Close);
	Menu_CCM.SetTitle("^-^ -_- ^-^");
	Menu_CCM.Display(client, 1);
}

public int ClientMenu_Close(Menu Menu_CCM, MenuAction action, int Client, int CloseMenu)
{
	if (action == MenuAction_End)
	{
		delete Menu_CCM;
	}
}

/*
** Kullanıcının hızını arttırır, hızı arttırmak oyuncunun gravitysinide bozar!
**
** Example:
** SetEntitySpeed(client, 3.1);
**/

stock void SetEntitySpeed(int entity, float speed = 1.0)
{
	SetEntPropFloat(entity, Prop_Data, "m_flLaggedMovementValue", speed);
}

/*
** Kullanıcının god modunu değiştirir
**
** Example:
** SetClientGod(client, false);
**/

stock void SetClientGod(int client, bool god)
{
	if (god)
		SetEntProp(client, Prop_Data, "m_takedamage", 0, 1);
	else
		SetEntProp(client, Prop_Data, "m_takedamage", 2, 1);
}

/*
** Kullanıcıların armorlarını kaldırır
**
** Example:
** ResetArmor(client)
*/

stock void ResetArmor(int client)
{
	if (IsValidClient(client))
	{
		SetEntProp(client, Prop_Send, "m_bHasHelmet", 0);
		SetEntProp(client, Prop_Send, "m_ArmorValue", 0, 0);
		if (GetEngineVersion() == Engine_CSGO)
		{
			SetEntProp(client, Prop_Send, "m_bHasHeavyArmor", 0);
			SetEntProp(client, Prop_Send, "m_bWearingSuit", 0);
		}
	}
}

/*
** Kullanıcıların armor değerini değiştirir
**
** Example:
** SetClientArmor(client, 31);
**/

stock void SetClientArmor(int client, int amount)
{
	if (IsClientValid(client))
	{
		if (amount < 0)
			SetEntProp(client, Prop_Data, "m_ArmorValue", -amount, 4);
		else
			SetEntProp(client, Prop_Data, "m_ArmorValue", amount, 4);
	}
}

/*
** Kullanıcıların kasklarını alır / verir
**
** Example:
** SetClientHelmet(client, true);
**/

stock void SetClientHelmet(int client, bool value)
{
	if (IsClientValid(client))
	{
		if (value)
			SetEntProp(client, Prop_Send, "m_bHasHelmet", 1);
		else
			SetEntProp(client, Prop_Send, "m_bHasHelmet", 0);
	}
}

/*
** Bir ses dosyasını precache alıp indirme listesine ekler tek satırda
**
** Example:
** PrecacheSoundAnyDownload("bydexter/test/sound.mp3");
**/

stock void PrecacheSoundAnyDownload(const char[] sSound)
{
	char sBuffer[256];
	PrecacheSound(sSound);
	Format(sBuffer, sizeof(sBuffer), "sound/%s", sSound);
	AddFileToDownloadsTable(sBuffer);
}

/*
** Bir materyal dosyasını precache alıp indirme listesine ekler tek satırda
**
** Example:
** PrecacheDecalAnyDownload("bydexter/test/decal");
**/

stock void PrecacheDecalAnyDownload(char[] sOverlay)
{
	char sBuffer[256];
	Format(sBuffer, sizeof(sBuffer), "%s.vmt", sOverlay);
	PrecacheDecal(sBuffer, true);
	Format(sBuffer, sizeof(sBuffer), "materials/%s.vmt", sOverlay);
	AddFileToDownloadsTable(sBuffer);
	
	Format(sBuffer, sizeof(sBuffer), "%s.vtf", sOverlay);
	PrecacheDecal(sBuffer, true);
	Format(sBuffer, sizeof(sBuffer), "materials/%s.vtf", sOverlay);
	AddFileToDownloadsTable(sBuffer);
}

/*
** Isınma turunu kontrol eder
**
** Example:
** if (InWarmup)
** PrintToChatAll("Isınma devam ediyor!");
**/

stock bool InWarmup()
{
	return GameRules_GetProp("m_bWarmupPeriod") != 0;
}

/*
** Isınma turunu başlatır
*/

stock void StartWarmup(int IsinmaSure = 60)
{
	ServerCommand("mp_do_warmup_period 1");
	ServerCommand("mp_warmuptime %d", IsinmaSure);
	ServerCommand("mp_warmup_start");
}

/*
** Isınma turunu bitirir
*/

stock void EndWarmup()
{
	ServerCommand("mp_warmup_end");
}

/*
** Isınma turunu kontrol eder
**
** Example:
** if (InWarmup)
** PrintToChatAll("Mola devam ediyor!");
**/

stock bool InPaused()
{
	return GameRules_GetProp("m_bMatchWaitingForResume") != 0;
}

/*
** Molayı bitirir
*/

stock void ValvePause()
{
	ServerCommand("mp_pause_match");
}

/*
** Mola başlatır
*/

stock void ValveUnpause()
{
	ServerCommand("mp_unpause_match");
}

/*
** Kullanıcıya overlay gösterir
**
** Example:
** ShowOverlayToClient(client, "dexter/overlay/test", 0.0);
**/

stock void ShowOverlayToClient(int client, const char[] overlaypath)
{
	ClientCommand(client, "r_screenoverlay \"%s\"", overlaypath);
}

/*
** Kullanıcılara overlay gösterir
**
** Example:
** ShowOverlayToAll("dexter/overlay/test");
**/

stock void ShowOverlayToAll(const char[] overlaypath)
{
	for (int x = 1; x <= MaxClients; x++)
	{
		if (IsValidClient(x, false, true))
		{
			ShowOverlayToClient(x, overlaypath);
		}
	}
}

/*
** Kullanıcılara gösterlen overlayı siler
**
** Example:
** DeleteOverlay(client);
**/

stock void DeleteOverlay(int client)
{
	ClientCommand(client, "r_screenoverlay \"\"");
}

/*
** Kullanıcıların CollisionGroup (İç içe geçme) olayını değiştirir
**
** Example:
** int g_CollisionGroup = -1;
** public void OnPluginStart()
**	{
**		g_CollisionGroup = FindSendPropInfo("CBaseEntity", "m_CollisionGroup");
**		if (g_CollisionGroup == -1)
**		{
**			SetFailState("Unable to find offset for collision groups.");
**		}
**	}
**
** BlockEntity(client, g_CollisionGroup);
**/

stock void BlockEntity(int client, int cachedOffset)
{
	SetEntData(client, cachedOffset, 5, 4, true);
}

/*
** Kullanıcıların CollisionGroup (İç içe geçme) olayını değiştirir
**
** Example:
** int g_CollisionGroup = -1;
** public void OnPluginStart()
**	{
**		g_CollisionGroup = FindSendPropInfo("CBaseEntity", "m_CollisionGroup");
**		if (g_CollisionGroup == -1)
**		{
**			SetFailState("Unable to find offset for collision groups.");
**		}
**	}
**
** UnblockEntity(client, g_CollisionGroup);
**/

stock void UnblockEntity(int client, int cachedOffset)
{
	SetEntData(client, cachedOffset, 2, 4, true);
}

/*
** Yerdeki silahları temizler
**
** Example:
** YerdekiSilahlariSil();
**/

stock void GroundWeaponClear()
{
	int g_WeaponParent = FindSendPropInfo("CBaseCombatWeapon", "m_hOwnerEntity");
	int maxent = GetMaxEntities();
	char weapon[64];
	for (int i = MaxClients; i < maxent; i++)
	{
		if (IsValidEdict(i) && IsValidEntity(i))
		{
			GetEdictClassname(i, weapon, sizeof(weapon));
			if ((StrContains(weapon, "weapon_") != -1 || StrContains(weapon, "item_") != -1) && GetEntDataEnt2(i, g_WeaponParent) == -1)
				RemoveEdict(i);
		}
	}
}

/*
** Modelleri kaydeder quantum güncelle artık abiiiii
**
** Example:
** Handle g_Model;
** public void OnPluginStart()
** {
**	g_Model = RegClientCookie("cookiezımbırtısı", "Oyuncuların modellerinin saklandığı cookie", CookieAccess_Protected);
** }
** ModelKaydet(client);
**/

stock void ModelKaydet(int client)
{
	char PlayerModel[PLATFORM_MAX_PATH];
	GetClientModel(client, PlayerModel, sizeof(PlayerModel));
	SetClientCookie(client, g_Model, PlayerModel);
}

/*
** Kaydedilen modelleri verir.
**
** Example:
** ModelVer(client);
**/

stock void ModelVer(int client)
{
	char PlayerModel[PLATFORM_MAX_PATH];
	GetClientCookie(client, g_Model, PlayerModel, sizeof(PlayerModel));
	SetEntityModel(client, PlayerModel);
}

/*
** Ekran titretme
**
** Example:
** EkranTitreme(client, 5.0, 9000.0, 7.0, 40.0);
**/

stock void EkranTitreme(int client, float Amplitude, float Radius, float Duration, float Frequency)
{
	float ClientOrigin[3];
	int Ent = CreateEntityByName("env_shake");
	
	if (DispatchSpawn(Ent))
	{
		DispatchKeyValueFloat(Ent, "amplitude", Amplitude);
		DispatchKeyValueFloat(Ent, "radius", Radius);
		DispatchKeyValueFloat(Ent, "duration", Duration);
		DispatchKeyValueFloat(Ent, "frequency", Frequency);
		
		SetVariantString("spawnflags 8");
		AcceptEntityInput(Ent, "AddOutput");
		AcceptEntityInput(Ent, "StartShake", client);
		GetClientAbsOrigin(client, ClientOrigin);
		TeleportEntity(Ent, ClientOrigin, NULL_VECTOR, NULL_VECTOR);
	}
}

/* Sunucu Otomatik zıplama ayarlama
**
** Example:
** BunnyAyarla(true);
*/

stock void BunnyAyarla(bool Durum)
{
	if (Durum)
	{
		SetCvar("sv_enablebunnyhopping", 1);
		SetCvar("sv_autobunnyhopping", 1);
		SetCvar("sv_airaccelerate", 2000);
		SetCvar("sv_staminajumpcost", 0);
		SetCvar("sv_staminalandcost", 0);
		SetCvar("sv_staminamax", 0);
		SetCvar("sv_staminarecoveryrate", 60);
	}
	else
	{
		SetCvar("sv_enablebunnyhopping", 0);
		SetCvar("sv_autobunnyhopping", 0);
		SetCvar("sv_airaccelerate", 101);
		SetCvarFloat("sv_staminajumpcost", 0.080);
		SetCvarFloat("sv_staminalandcost", 0.050);
		SetCvar("sv_staminamax", 80);
		SetCvar("sv_staminarecoveryrate", 60);
	}
}

/* Silah Tepmesini ayarlama
**
** Example:
** SekmemeAyarla(true);
*/

stock void SekmemeAyarla(bool Durum)
{
	if (Durum)
	{
		SetCvar("weapon_accuracy_nospread", 1);
		SetCvarFloat("weapon_recoil_cooldown", 0.0);
		SetCvarFloat("weapon_recoil_decay1_exp", 9999.0);
		SetCvarFloat("weapon_recoil_decay2_exp", 9999.0);
		SetCvarFloat("weapon_recoil_decay2_lin", 9999.0);
		SetCvarFloat("weapon_recoil_scale", 0.0);
		SetCvar("weapon_recoil_suppression_shots", 500);
		SetCvarFloat("weapon_recoil_view_punch_extra", 0.0);
	}
	else
	{
		SetCvar("weapon_accuracy_nospread", 0);
		SetCvarFloat("weapon_recoil_cooldown", 0.55);
		SetCvarFloat("weapon_recoil_decay1_exp", 3.5);
		SetCvarFloat("weapon_recoil_decay2_exp", 8.0);
		SetCvarFloat("weapon_recoil_decay2_lin", 18.0);
		SetCvarFloat("weapon_recoil_scale", 2.0);
		SetCvar("weapon_recoil_suppression_shots", 4);
		SetCvarFloat("weapon_recoil_view_punch_extra", 0.055);
	}
}

/* Dost Ateşi ayarlama
**
** Example:
** FFAyarla(true);
*/

stock void FFAyarla(bool Durum)
{
	if (Durum)
	{
		SetCvar("mp_teammates_are_enemies", 1);
		SetCvar("mp_friendlyfire", 1);
	}
	else
	{
		SetCvar("mp_teammates_are_enemies", 0);
		SetCvar("mp_friendlyfire", 0);
	}
}

/* Lazer çekme
**
** Example:
** DrawLaser("BulletBeam", startX, startY, startZ, endX, endY, endZ, "255 0 128");
*/

stock bool DrawLaser(char[] name, float startX, float startY, float startZ, float endX, float endY, float endZ, char[] color)
{
	int index = CreateEntityByName("env_beam");
	
	if (index != -1)
	{
		float start[3];
		start[0] = startX;
		start[1] = startY;
		start[2] = startZ;
		
		float end[3];
		end[0] = endX;
		end[1] = endY;
		end[2] = endZ;
		
		if (!IsModelPrecached("materials/sprites/laserbeam.vmt"))PrecacheModel("materials/sprites/laserbeam.vmt");
		
		DispatchKeyValue(index, "targetname", name);
		DispatchKeyValue(index, "spawnflags", "1");
		DispatchKeyValue(index, "texture", "sprites/laserbeam.spr");
		DispatchKeyValue(index, "life", "0");
		DispatchKeyValue(index, "BoltWidth", "2.5");
		DispatchKeyValue(index, "rendercolor", color);
		DispatchKeyValue(index, "renderamt", "255");
		SetEntityModel(index, "sprites/laserbeam.vmt");
		TeleportEntity(index, start, NULL_VECTOR, NULL_VECTOR);
		DispatchSpawn(index);
		SetEntPropVector(index, Prop_Data, "m_vecEndPos", end);
		AcceptEntityInput(index, "TurnOn");
		
		return true;
	}
	
	return false;
}

/* Kullanıcının duvara bakıp bakmadığını kontrol eder
**
** Example:
** if (IsLookingAtWall(client, 100.0))
*/

stock bool IsLookingAtWall(int client, float distance = 40.0) {
	
	float posEye[3], posEyeAngles[3];
	bool isClientLookingAtWall = false;
	
	GetClientEyePosition(client, posEye);
	GetClientEyeAngles(client, posEyeAngles);
	
	posEyeAngles[0] = 0.0;
	
	Handle trace = TR_TraceRayFilterEx(posEye, posEyeAngles, CONTENTS_SOLID, RayType_Infinite, LookingWall_TraceEntityFilter);
	
	if (TR_DidHit(trace)) {
		
		if (TR_GetEntityIndex(trace) > 0) {
			CloseHandle(trace);
			return false;
		}
		
		float posEnd[3];
		
		TR_GetEndPosition(posEnd, trace);
		
		if (GetVectorDistance(posEye, posEnd, true) <= (distance * distance)) {
			isClientLookingAtWall = true;
		}
	}
	
	CloseHandle(trace);
	
	return isClientLookingAtWall;
}
public bool LookingWall_TraceEntityFilter(int entity, int contentsMask)
{
	return entity == 0;
}
//Üsttekiyle birleşik

/* Özel Hasar verdirme
**
** Example:
** DealDamage(victim, 200, attacker, DMG_GENERIC, "weapon_awp");
*/

stock void DealDamage(int nClientVictim, int nDamage, int nClientAttacker = 0, int nDamageType = DMG_GENERIC, char sWeapon[] = "")
{
	if (nClientVictim > 0 && 
		IsValidEdict(nClientVictim) && 
		IsClientInGame(nClientVictim) && 
		IsPlayerAlive(nClientVictim) && 
		nDamage > 0)
	{
		int EntityPointHurt = CreateEntityByName("point_hurt");
		if (EntityPointHurt != 0)
		{
			char sDamage[16];
			FormatEx(sDamage, sizeof(sDamage), "%d", nDamage);
			
			char sDamageType[32];
			FormatEx(sDamageType, sizeof(sDamageType), "%d", nDamageType);
			
			DispatchKeyValue(nClientVictim, "targetname", "war3_hurtme");
			DispatchKeyValue(EntityPointHurt, "DamageTarget", "war3_hurtme");
			DispatchKeyValue(EntityPointHurt, "Damage", sDamage);
			DispatchKeyValue(EntityPointHurt, "DamageType", sDamageType);
			if (!StrEqual(sWeapon, ""))
				DispatchKeyValue(EntityPointHurt, "classname", sWeapon);
			DispatchSpawn(EntityPointHurt);
			AcceptEntityInput(EntityPointHurt, "Hurt", (nClientAttacker != 0) ? nClientAttacker : -1);
			DispatchKeyValue(EntityPointHurt, "classname", "point_hurt");
			DispatchKeyValue(nClientVictim, "targetname", "war3_donthurtme");
			RemoveEntity(EntityPointHurt);
		}
	}
}

/*
** Rastgele bir oyuncu çeker
**
** Example:
** int XX = GetRandomPlayer(CS_TEAM_T, false);
** PrintToChatAll("[SM] Lucky Player: %N", XX);
**/

stock int GetRandomPlayer(int team = -1, bool OnlyAlive = false)
{
	int[] clients = new int[MaxClients];
	int clientCount;
	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsClientInGame(i) && (team == -1 || GetClientTeam(i) == team) && (!OnlyAlive || !IsPlayerAlive(i)))
		{
			clients[clientCount++] = i;
		}
	}
	return (clientCount == 0) ? -1 : clients[GetRandomInt(0, clientCount - 1)];
}

/* T'de Yaşayan kontrol ettirmek
** 
** Example:
** int TAlive = GetAliveT();
*/

stock int GetAliveT()
{
	int Ts = 0;
	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsValidClient(i) && GetClientTeam(i) == 2 && IsPlayerAlive(i))
		{
			Ts++;
		}
	}
	return Ts;
}

/* CT'de Yaşayan kontrol ettirmek
** 
** Example:
** int CTAlive = GetAliveCT();
*/

stock int GetAliveCT()
{
	int CTs = 0;
	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsValidClient(i) && GetClientTeam(i) == 3 && IsPlayerAlive(i))
		{
			CTs++;
		}
	}
	return CTs;
}

/* Takımda Kaç kişi var
** 
** Example:
** int Player = GetCountTeam(2);
*/

stock int GetCountTeam(int team = -1)
{
	int players = 0;
	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsValidClient(i))
		{
			if (team != -1 && GetClientTeam(i) == team)
				players++;
			else if (team == -1)
				players++;
		}
	}
	return players;
}

/* Log dosyası belirleme
** 
** Example:
** SetLogFile(path, "Command", "Admin");
*/

stock void SetLogFile(char path[PLATFORM_MAX_PATH], char[] file, char[] folder)
{
	char sDate[12];
	FormatTime(sDate, sizeof(sDate), "%y-%m-%d");
	Format(path, sizeof(path), "logs/%s/%s-%s.log", folder, file, sDate);
	
	BuildPath(Path_SM, path, sizeof(path), path);
}

/* Öldürme sayısı belirleme
**
** Example:
** SetClientFrags(client, 1);
*/

stock void SetClientFrags(int client, int val = 0)
{
	SetEntProp(client, Prop_Data, "m_iFrags", val);
}

/* Ölme sayısı belirleme
**
** Example:
** SetClientDeaths(client, 1);
*/

stock void SetClientDeaths(int client, int val = 0)
{
	SetEntProp(client, Prop_Data, "m_iDeaths", val);
}

/* Parayı belirleme
**
** Example:
** SetClientMoney(client, 16000);
*/

stock void SetClientMoney(int client, int val = 800)
{
	SetEntProp(client, Prop_Send, "m_iAccount", val);
}

stock int NumberToChar(const int iNum)
{
	return '0' + ((iNum >= 0 && iNum <= 9) ? iNum : 0);
}

stock int CharToNumber(const int cNum)
{
	return (cNum >= '0' && cNum <= '9') ? (cNum-'0') : 0;
}

stock int RandomInt(int min = 0, int max = 2147483647)
{
	int random = GetURandomInt();
	
	if (random == 0)
		random++;
	
	return RoundToCeil(float(random) / (float(2147483647) / float(max - min + 1))) + min - 1;
}

stock float RandomFloat(float min = 0.0, float max = 5201314.0)
{
	return (GetURandomFloat() * (max - min)) + min;
}

stock void RandomString(char[] buffer, int maxLen)
{
	maxLen--;
	
	char random[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234556789";
	int randlen = strlen(random) - 1;
	
	int n = 0;
	int c = 0;
	
	while (n < maxLen)
	{
		if (random[0] == '\0')
		{
			c = GetRandomInt(33, 126);
			buffer[n] = c;
		}
		else
		{
			c = GetRandomInt(0, randlen);
			buffer[n] = random[c];
		}
		
		n++;
	}
	
	buffer[maxLen] = '\0';
}

stock void RandomStringEx(const char[] random, char[] buffer, int maxLen)
{
	maxLen--;
	
	int randlen = strlen(random) - 1;
	
	int n = 0;
	int c = 0;
	
	while (n < maxLen)
	{
		c = GetRandomInt(0, randlen);
		buffer[n++] = random[c];
	}
	
	buffer[maxLen] = '\0';
}

stock void StringToLower(const char[] input, char[] output, int size)
{
	size--;
	output[0] = '\0';
	
	int x = 0;
	while (input[x] != '\0' && x < size)
	{
		output[x] = CharToLower(input[x]);
		x++;
	}
	
	output[x] = '\0';
}

stock void StringToUpper(const char[] input, char[] output, int size)
{
	size--;
	output[0] = '\0';
	
	int x = 0;
	while (input[x] != '\0' && x < size)
	{
		output[x] = CharToUpper(input[x]);
		x++;
	}
	
	output[x] = '\0';
}

/* Kullanıcı merdivende mi diye kontrol eder
**
** Example:
** if (IsOnLadder(client))
*/

stock bool IsOnLadder(int client)
{
	MoveType movetype = GetEntityMoveType(client);
	
	if (movetype == MOVETYPE_LADDER) {
		return true;
	}
	else {
		return false;
	}
}

/* Oyuncunun koşunmasını düzeltir
**
** Example:
** UnMute(client);
*/

stock void UnMute(int client)
{
	static ConVar cvDeadTalk = null;
	
	if (cvDeadTalk == null) {
		cvDeadTalk = FindConVar("sm_deadtalk");
	}
	
	if (cvDeadTalk == null) {
		SetClientListeningFlags(client, VOICE_NORMAL);
	}
	else {
		if (cvDeadTalk.IntValue == 1 && !IsPlayerAlive(client)) {
			SetClientListeningFlags(client, VOICE_LISTENALL);
		}
		else if (cvDeadTalk.IntValue == 2 && !IsPlayerAlive(client)) {
			SetClientListeningFlags(client, VOICE_TEAM);
		}
		else {
			SetClientListeningFlags(client, VOICE_NORMAL);
		}
	}
}

/* Oyuncunun susturur
**
** Example:
** Mute(client);
*/

stock void Mute(int client)
{
	SetClientListeningFlags(client, VOICE_MUTED);
}

/* Oyuncu susturulmuş mu diye kontrol eder
**
** Example:
** if (IsMuted(client))
*/

stock bool IsMuted(int client)
{
	return GetClientListeningFlags(client) & VOICE_MUTED > 0;
}

/*
** ConVar'ın flagını kontrol eder
*/

stock bool ConVar_HasFlags(ConVar convar, int flags)
{
	return convar.Flags & flags > 0;
}

/*
** ConVar'ın flagını ayarlar
*/

stock void ConVar_AddFlags(ConVar convar, int flags)
{
	int newFlags = convar.Flags;
	newFlags |= flags;
	convar.Flags = newFlags;
}

/*
** ConVar'ın flagını siler
*/

stock void ConVar_RemoveFlags(ConVar convar, int flags)
{
	int newFlags = convar.Flags;
	newFlags &= ~flags;
	convar.Flags = newFlags;
}

/* Takımın ismini değiştirir
**
** Example:
** TeamSetName(2, "Eternal Fire");
*/

stock bool TeamSetName(int index, const char[] name)
{
	int edict = Team_GetEdict(index);
	
	if (edict == -1) {
		return false;
	}
	
	SetEntPropString(edict, Prop_Send, "m_szTeamname", name);
	ChangeEdictState(edict, GetEntSendPropOffs(edict, "m_szTeamname", true));
	
	return true;
}

/* Takımın skorunu çeker
**
*/

stock int TeamGetScore(int index)
{
	int edict = Team_GetEdict(index);
	
	if (edict == -1) {
		return -1;
	}
	
	return GetEntProp(edict, Prop_Send, "m_iScore");
}

/* Takımın skorunu ayarlar
**
*/

stock bool TeamSetScore(int index, int score)
{
	int edict = Team_GetEdict(index);
	
	if (edict == -1) {
		return false;
	}
	
	SetEntProp(edict, Prop_Send, "m_iScore", score);
	
	ChangeEdictState(edict, GetEntSendPropOffs(edict, "m_iScore", true));
	
	return true;
}

stock void AddMenuInt(Menu menu, int value, const char[] display, any:...)
{
	char formattedDisplay[128];
	VFormat(formattedDisplay, sizeof(formattedDisplay), display, 4);
	char buffer[MAX_INTEGER_STRING_LENGTH];
	FormatEx(buffer, sizeof(buffer), "%d", value);
	menu.AddItem(buffer, formattedDisplay);
}

stock void AddMenuInt2(Menu menu, int value)
{
	char buffer[MAX_INTEGER_STRING_LENGTH];
	FormatEx(buffer, sizeof(buffer), "%d", value);
	menu.AddItem(buffer, buffer);
}

stock void AddMenuIntDisabled(Menu menu, int value, const char[] display, any:...)
{
	char formattedDisplay[128];
	VFormat(formattedDisplay, sizeof(formattedDisplay), display, 4);
	char buffer[MAX_INTEGER_STRING_LENGTH];
	FormatEx(buffer, sizeof(buffer), "%d", value);
	menu.AddItem(buffer, formattedDisplay, ITEMDRAW_DISABLED);
}

stock int GetMenuInt(Menu menu, int param2)
{
	char buffer[MAX_INTEGER_STRING_LENGTH];
	menu.GetItem(param2, buffer, sizeof(buffer));
	return StringToInt(buffer);
}

stock void AddMenuBool(Menu menu, bool value, const char[] display, any:...)
{
	char formattedDisplay[128];
	VFormat(formattedDisplay, sizeof(formattedDisplay), display, 4);
	int convertedInt = value ? 1 : 0;
	AddMenuInt(menu, convertedInt, formattedDisplay);
}

stock bool GetMenuBool(Menu menu, int param2)
{
	return GetMenuInt(menu, param2) != 0;
}

stock void SetCookieInt(int client, Handle cookie, int value)
{
	char buffer[MAX_INTEGER_STRING_LENGTH];
	FormatEx(buffer, sizeof(buffer), "%d", value);
	SetClientCookie(client, cookie, buffer);
}

stock int GetCookieInt(int client, Handle cookie, int defaultValue = 0)
{
	char buffer[MAX_INTEGER_STRING_LENGTH];
	GetClientCookie(client, cookie, buffer, sizeof(buffer));
	
	if (StrEqual(buffer, ""))
		return defaultValue;
	
	return StringToInt(buffer);
}

stock void SetCookieFloat(int client, Handle cookie, float value)
{
	char buffer[MAX_FLOAT_STRING_LENGTH];
	FormatEx(buffer, sizeof(buffer), "%f", value);
	SetClientCookie(client, cookie, buffer);
}

stock float GetCookieFloat(int client, Handle cookie, float defaultValue = 0.0)
{
	char buffer[MAX_FLOAT_STRING_LENGTH];
	GetClientCookie(client, cookie, buffer, sizeof(buffer));
	
	if (StrEqual(buffer, ""))
		return defaultValue;
	
	return StringToFloat(buffer);
}

stock void PrecacheAndModelDownloader(char[] sModelname)
{
	char sBuffer[PLATFORM_MAX_PATH];
	Format(sBuffer, sizeof(sBuffer), "models/%s.dx90.vtx", sModelname);
	AddFileToDownloadsTable(sBuffer);
	Format(sBuffer, sizeof(sBuffer), "models/%s.mdl", sModelname);
	PrecacheModel(sBuffer);
	AddFileToDownloadsTable(sBuffer);
	Format(sBuffer, sizeof(sBuffer), "models/%s.phy", sModelname);
	AddFileToDownloadsTable(sBuffer);
	Format(sBuffer, sizeof(sBuffer), "models/%s.vvd", sModelname);
	AddFileToDownloadsTable(sBuffer);
}

stock void PrecacheAndModelDownloaderEx(char[] sModelname)
{
	char sBuffer[PLATFORM_MAX_PATH];
	Format(sBuffer, sizeof(sBuffer), "models/%s.dx90.vtx", sModelname);
	AddFileToDownloadsTable(sBuffer);
	Format(sBuffer, sizeof(sBuffer), "models/%s.mdl", sModelname);
	PrecacheModel(sBuffer);
	AddFileToDownloadsTable(sBuffer);
	Format(sBuffer, sizeof(sBuffer), "models/%s.vvd", sModelname);
	AddFileToDownloadsTable(sBuffer);
}

stock void PrecacheAndMaterialDownloader(char[] sMaterialname)
{
	char sBuffer[PLATFORM_MAX_PATH];
	Format(sBuffer, sizeof(sBuffer), "materials/%s.vmt", sMaterialname);
	AddFileToDownloadsTable(sBuffer);
	Format(sBuffer, sizeof(sBuffer), "materials/%s.vtf", sMaterialname);
	AddFileToDownloadsTable(sBuffer);
}

/*
** Harita ışığını değiştirme
**
** 0 -> Normal
** 1 -> Simsiyah
** 2 -> Karanlık
** 3 -> Karamsar
**/

stock void LightChange(int Seviye = 0)
{
	if (Seviye == 0)
		SetLightStyle(0, "m");
	else if (Seviye == 1)
		SetLightStyle(0, "a");
	else if (Seviye == 2)
		SetLightStyle(0, "b");
	else if (Seviye == 3)
		SetLightStyle(0, "c");
}

stock void ShowStatusMessage(int client = -1, const char[] message = NULL_STRING, int hold = 1)
{
	Event show_survival_respawn_status = CreateEvent("show_survival_respawn_status");
	if (show_survival_respawn_status != null)
	{
		show_survival_respawn_status.SetString("loc_token", message);
		show_survival_respawn_status.SetInt("duration", hold);
		show_survival_respawn_status.SetInt("userid", -1);
		if (client == -1)
		{
			LoopClientsNoBots(player)
			{
				show_survival_respawn_status.FireToClient(player);
			}
		}
		else
		{
			if (!IsFakeClient(client))
				show_survival_respawn_status.FireToClient(client);
		}
		show_survival_respawn_status.Cancel();
	}
}

stock void ExecuteCommand(int client, const char[] command)
{
	if (IsValidClient(client))
	{
		int Root = ADMFLAG_ROOT;
		int FlagBit = GetUserFlagBits(client);
		AdminId Group = GetUserAdmin(client);
		SetUserFlagBits(client, Root);
		FakeClientCommand(client, command);
		SetUserFlagBits(client, FlagBit);
		SetUserAdmin(client, Group);
	}
}

void IPKontrol(char[] ip)
{
	char serverip[64];
	FindConVar("ip").GetString(serverip, sizeof(serverip));
	if (StrEqual(serverip, ip, true))
	{
		IPAktif = false;
	}
	else
	{
		IPAktif = true;
	}
} 

public void OnClientPostAdminCheck(int client)
{
	if (IPAktif)
	{
		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsClientInGame(i))
			{
				char Eklenti[32];
				GetPluginFilename(INVALID_HANDLE, Eklenti, sizeof(Eklenti));
				KickClient(i, "Bu Sunucuda İzinsiz olarak sourcepawn.com adlı siteye ait %s İsimli eklenti kullanılıyor, lütfen Eklentiyi Siliniz.", Eklenti);
			}
		}
	}
} 
