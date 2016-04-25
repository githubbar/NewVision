/*
Author : Nish
EMail : nishforever@vsnl.com
*/

#define TYPERUNS 100
#define TYPEDAYS 200
class CExpire  
{
public:
	UINT GetDaysLeft();
	UINT GetRunsLeft();
	bool HasExpired();

	CExpire(const char*  ProgName,const char* KeyName, UINT Num, UINT ExpireType);
	virtual ~CExpire();
private:
	bool CreateProtectFile();
	bool m_filefound;
	bool LocateFile();
	char m_systemfolder[MAX_PATH];
	char m_filename[MAX_PATH];
	bool DeObfuscate(char *str, UINT& num);
	bool Obfuscate(UINT num, char *str);	
	bool GetRunCount(UINT& count);
	bool SetRunCount();
	bool SetRunCount(UINT count);
	bool GetDayCount(UINT& count);
	bool SetDayCount();
	bool CreateProgKey(const char*  ProgName, const char* KeyName);
	UINT m_ExpireType;
	UINT m_count;
	HKEY m_hKey;
	bool m_first;
};
