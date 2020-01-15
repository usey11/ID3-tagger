# ID3-tagger
Simple header only library to write IDv1 tags to MP3 files

How to use:
```
ID3V1 mp3(file_name);
mp3.printTags();

auto tags = mp3.getTags();

tags.title = "Never Gonna Give You Up";

mp3.setTags(tags);
mp3.writeTags();
```
