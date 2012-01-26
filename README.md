# TSLwatch

TSLwatch is a program application that downloads [EU Trusted Lists of Certification Service Providers](http://ec.europa.eu/information_society/policy/esignature/eu_legislation/trusted_lists/index_en.htm) and installs root certificates found there into Windows certificate store.

# Process

Currently it only works on local files and without signature validation. The code is in alpha stage.

* TODO: Open TSL URL (https://ec.europa.eu/information_society/policy/esignature/trusted-list/tl-mp.xml)
	- TODO: Validate TSL signature
	- Iterate through tsl:PointersToOtherTSL
	- For each tsl:OtherTSLPointer
		- Country TSL URL in tsl:TSLLocation
			- TSLs in XML format have tslx:MimeType application/vnd.etsi.tsl+xml
		- Country TSL signing certificate in tsl:X509Certificate
		- For each country TSL URL
			- TODO: Fetch TSL URL
			- TODO: Validate TSL signature
			- Iterate through tsl:TrustServiceProviderList
			- For each tsl:TrustServiceProvider
				- Certificate is in tsl:X509Certificate
				- Certificate identifier is in tsl:X509SubjectName
				- Check if this certificate is installed in certificate system store
					- If not, install it				
* Use tsl:NextUpdate to schedule next run of TSLwatch