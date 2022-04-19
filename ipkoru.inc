/*
**
*/
#if defined _ipkoru_included
 #endinput
#endif
#define _ipkoru_included

bool IPAktif;

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
