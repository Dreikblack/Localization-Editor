**Localization Editor** is a tool for creating and editing localization/language files which your game or application can use to support several languages.

You can also find a lot of custom widgets for Ultra Engine in Source\UI\CustomWidgets

![](https://media.discordapp.net/attachments/1216537215434358865/1318999280891068486/Localization_Editor_3.gif?ex=67645d4e&is=67630bce&hm=65f6e560296fe6e232bcbc2fe4dcd621acea7239a504a8f7b6a91aecae435962&=&width=1257&height=670)

 - Localization Editor loads all .local files in folder when you open or
   create one.
 - You can use names like map1.English.local and then only map1 files   
   will be loaded for editing.
 - .local format is pretty simple:
>     key1=content1
>     key2=content1\ncontent2
 - \n used for multi lines texts, but in this tool you can just tap
   Enter to make a new line.
 - Double click on table string line to open edit string line dialog.
 - Find specific line quickly by typing a characters that key contains
   in the filter field, cross button to show all lines
 - You can switch between loaded language localizations with combo box
   next to "Current Localization" label.
 - Once you done remember to save changes to localization files with
   "Save All" button
 - Parsing a local file in a code:

```cpp
auto stream = ReadFile(path);
if (!stream) {
    return newLocalMap;
}
WString text = stream->ReadWString();
//map key-content
std::map<WString, WString> newLocalMap;
vector<WString> textStrings = text.Split("\r\n");
for (WString line : textStrings) {
    vector<WString> entitiyProperties = line.Split("=");
    if (entitiyProperties.size() > 1) {
        WString newLine = entitiyProperties[1].Replace("\\n", "\n");
        newLocalMap[entitiyProperties[0]] = newLine;
    } else {
        newLocalMap[entitiyProperties[0]] = "";
    }
}
stream->Close();
```
