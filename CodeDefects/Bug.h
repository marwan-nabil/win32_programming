#pragma once

#include <windows.h>

// Function prototypes
bool CheckDomain(wchar_t const *);
HRESULT ReadUserAccount();

// These constants define the common sizes of the
// user account information throughout the program
const int USER_ACCOUNT_LEN = 256;
const int ACCOUNT_DOMAIN_LEN = 128;