// Thanks to jghali
#ifdef _WIN32

#include <windows.h>
#include <shlobj.h>

QString getWin32SystemFontDir()
{
	QString qstr;
	WCHAR dir[512];
	if ( SHGetSpecialFolderPathW(nullptr, dir, CSIDL_FONTS, false) )
	{
		qstr = QString::fromWCharArray(dir);
		if( !qstr.endsWith(QLatin1Char('\\')) )
			qstr += QLatin1Char('\\');
		qstr.replace( QLatin1Char('\\'), QLatin1Char('/') );
	}
	return qstr;
}


#endif
