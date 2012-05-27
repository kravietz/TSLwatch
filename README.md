# TSLwatch

TSLwatch is a program application that downloads [EU Trusted Lists of Certification Service Providers](http://ec.europa.eu/information_society/policy/esignature/eu_legislation/trusted_lists/index_en.htm) and installs root certificates found there into Windows certificate store.

# Process

Currently it only works on local files and without signature validation. The code is in alpha stage.

* Open TSL URL (https://ec.europa.eu/information_society/policy/esignature/trusted-list/tl-mp.xml)
	- TODO: Validate TSL signature
	- For each tsl:OtherTSLPointer
		- Country TSL URL in tsl:TSLLocation
			- TSLs in XML format have tslx:MimeType application/vnd.etsi.tsl+xml
		- Country TSL signing certificate in tsl:X509Certificate
		- For each country TSL URL
			- Fetch TSL URL
			- TODO: Validate TSL signature
			- Iterate through tsl:TrustServiceProviderList
			- For each tsl:TrustServiceProvider
				- Certificate is in tsl:X509Certificate
				- Certificate identifier is in tsl:X509SubjectName
				- Check if this certificate is installed in certificate system store
					- If not, install it				
* TODO: Use tsl:NextUpdate to schedule next run of TSLwatch
* TODO: Fix EventLog reporting

# Installer
HKLM\System\CurrentControlSet\Services\eventlog\Application\TSLwatch
TypesSupported (REG_DWORD) = 7
EventMessageFile (REG_EXPAND_SZ) 

Currently added to Events.reg

# Development resources

* ETSI TS 102 200 (http://www.etsi.org/deliver/etsi_ts/102200_102299/102231/03.01.02_60/ts_102231v030102p.pdf)
* Microsoft XPath examples  (http://msdn.microsoft.com/en-us/library/ms256086.aspx)
* MSXML selectNodes method (http://msdn.microsoft.com/en-us/library/windows/desktop/ms754523(v=vs.85).aspx)
* Compiler Errors When You Use #import with XML in Visual C++ .NET (http://support.microsoft.com/kb/316317)
