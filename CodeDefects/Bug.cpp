#include "Bug.h"

// the user account
wchar_t g_userAccount[USER_ACCOUNT_LEN] = {L"domain\\user"};
int len = 0;

bool CheckDomain(wchar_t const *domain)
{
    return (wcsnlen_s(domain, USER_ACCOUNT_LEN) > 0);
}

HRESULT ReadUserAccount()
{
    return S_OK;
}

bool ProcessDomain()
{
    wchar_t *domain = new wchar_t[ACCOUNT_DOMAIN_LEN];
    // ReadUserAccount gets a 'domain\user' input from
    //the user into the global 'g_userAccount'
    if(SUCCEEDED(ReadUserAccount()))
    {
        // Copies part of the string prior to the '\'
        // character onto the 'domain' buffer
        for(len = 0; (len < ACCOUNT_DOMAIN_LEN) && (g_userAccount[len] != L'\0'); len++)
        {
            if(g_userAccount[len] == L'\\')
            {
                // Stops copying on the domain and user separator ('\')
                break;
            }
            domain[len] = g_userAccount[len];
        }
        if((len == ACCOUNT_DOMAIN_LEN) || (g_userAccount[len] != L'\\'))
        {
            // '\' was not found. Invalid domain\user string.
            delete[] domain;
            return false;
        }
        else
        {
            domain[len] = L'\0';
        }
        // Process domain string
        bool result = CheckDomain(domain);

        delete[] domain;
        return result;
    }
    return false;
}

int path_dependent(int n)
{
    int i = 0;
    int j = 0;
    if(n == 0)
        i = 1;
    else
        j = 1;
    return i + j;
}