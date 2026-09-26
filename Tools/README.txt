A quick-documentation about tools

DESUtil.exe
- Being used to encrypt passwords for .opt files (CaptainHerlock configuration)
- Usage:
1) Open it (Just doubleclick)
2) Type your password in the "Decrypted Text" field
3) Click "Encrypt"
4) Copy result from "Encrypted Text" field
5) Paste it to your .opt
6) Done (Don't forget to compress)


RappelzCompress_v2.exe
- Being used to compress .opt files (CaptainHerlock configuration)
- Without compression, your Herlock will not see the changes you've made in your opt
- Usage:
1) Open it (just double-click). Make sure zlib1.dll is exist in the same directory as executable (otherwise it just will throw an error)
2) Open directory with your .opt file
3) Click on your .opt file
4) Drag your .opt file to the window of RappelzCompress tool
5) Done


GlanduRDBTool
- Being used to make RDB files for your client
- Without rdb files your client will not see some changes you've made on the server
- Usage:
1) Start RappelzRDBToolQt.exe
2) Click "SQL Options"
3) Configure your server ip, port and account.
-- Default is 127.0.0.1 IP and port is blank (if you did not change anything)
-- Default account is sa without password (if you didn't change anything during SQL server installation). But that's insecure, so better use other account
4) Do NOT uncheck "Save password"; Click "OK"
5) Select database structure you want to update
6) Click "File" -> "Load from SQL Database"
7) In the appeared window, change "Arcadia" to "ArcadiaKitekat" (if you didn't change its name)
8) Click "OK"
9) Wait until tool would load the table
10) Click "Save to File"
11) Navigate the path to your game client / Resource directory (Or dump, wherever you want to put this RDB in)
12) Done


Grimoire
- Being used to create RDB files for client (but structs here are for 7.2 only); for dumping/packing client data; hashing resources and other useful things
- Usage (to work with data)
-- Dump client:
1) Start Grimoire.exe
2) In the "New" field select "DATA"
3) Click "Load"
4) Navigate to your game client directory with data.xxx files
5) Select data.000 file
6) Click "Open"
7) Right side; In "Extensions" right click to "all"
8) Click "Export"
9) Wait until done
10) After dumping client, look in the Grimoire / Output directory. Everything would be here
-- Build new datas:
1) Start Grimoire.exe
2) In the "New" field select "DATA"
3) Click "New"
4) Navigate to your dump directory
5) Click "OK"
6) Wait until packing is completed. Grimoire would notify you
7) Packed data files would be located in the Grimoire / Output directory


Scripts:
Every script has documentation inside of it. Just open script using any text editor (I would suggest using Notepad++)