#include <bits/stdc++.h>
#include <windows.h>
#include <cstring>
HWND hwnd;
int main()
{
	system("taskkill /f /t /im systemloader.exe");
	char* username;
    char destinationPath[MAX_PATH];
	username = getenv("USERPROFILE");
	snprintf(destinationPath, sizeof(destinationPath), "%s\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\systemloader.exe", username);
	if (remove(destinationPath) == 0)
	{
		MessageBox(hwnd, TEXT("���"), TEXT("ɾ�����"), MB_OK);
	}
	else
	{
		MessageBox(hwnd,TEXT("ʧ��"),TEXT("û���ҵ�����"),MB_OK);
	}
}
