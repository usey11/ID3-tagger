#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

struct ID3V1Tags {
	std::string title;
	std::string artist;
	std::string album;
	std::string year;
	std::string comment;
	int track;
	int genre; 
	void print() {
			auto printTag = [&](std::string tagName, std::string& tag) {
				std::cout << tagName << ": " << tag << std::endl;
			};
			printTag("Title", title);
			printTag("Artist", artist);
			printTag("Album", album);
			printTag("Year", year);
			printTag("Comment", comment);
		};// Convert to enum
	};

class ID3V1 {
public:
	ID3V1(std::string filename_)
		:filename(filename_) {
		tagsRead = readTags();
	};


	bool readTags() {
		// Open the file
		std::ifstream file;
		file.open(filename, std::ios::binary);

		// Try to open the file
		if (!file.is_open()) {
			std::cout << "File couldn't be opened!";
			return false;
		}

		// Header
		char header[HEADER_LENGTH + 1] = { 0 };

		file.seekg(HEADER_OFFSET,std::ios::end);
		file.read(header, HEADER_LENGTH);

		// Header isn't there so no tags written
		if (std::string(header) != HEADER) {
			return false;
		}

		// Title
		char title[TITLE_LENGTH + 1] = { 0 };

		file.seekg(TITLE_OFFSET, std::ios::end);
		file.read(title, TITLE_LENGTH);
		tags.title = std::string(title);

		// Artist
		char artist[ARTIST_LENGTH + 1] = { 0 };

		file.seekg(ARTIST_OFFSET, std::ios::end);
		file.read(artist, ARTIST_LENGTH);
		tags.artist = std::string(artist);

		// Album
		char album[ALBUM_LENGTH + 1] = { 0 };

		file.seekg(ALBUM_OFFSET, std::ios::end);
		file.read(album, ALBUM_LENGTH);
		tags.album = std::string(album);

		// Year
		char year[YEAR_LENGTH + 1] = { 0 };

		file.seekg(YEAR_OFFSET, std::ios::end);
		file.read(year, YEAR_LENGTH);
		tags.year = std::string(year);

		// Comment
		char comment[COMMENT_LENGTH + 1] = { 0 };

		file.seekg(COMMENT_OFFSET, std::ios::end);
		file.read(comment, COMMENT_LENGTH);
		tags.comment = std::string(comment);

		// The track is stored int hte last two bytes of the comment
		// If the second last byte is 0 then a track number can be there

		if (comment[28] == 0) {
			// There could be a track number
			tags.track = int(comment[29]);
		}
		else {
			tags.track = 0;
		}

		// Genre
		char genre[1] = { 0 };

		file.seekg(GENRE_OFFSET, std::ios::end);
		file.read(genre, GENRE_OFFSET);
		tags.genre = genre[0];


		file.close();
		return true;
	};

	ID3V1Tags getTags() {
		return tags;
	};

	void setTags(ID3V1Tags tags_) {
		tags = tags_;
	};

	void printTags() {
		tags.print();
	};

	bool writeTags() {
		// Open the file
		std::fstream file;
		file.open(filename, std::ios::out | std::ios::in | std::ios::binary | std::ios::ate);

		// Check if it opened correctly
		if (!file.is_open()) {
			return false;
		}

		auto writeData = [&file](int offset, int length, std::string data) {
			char rData[32] = { 0 };
			data.copy(rData, 32);
			file.seekp(offset, std::ios::end);
			file.write(rData, length);
		};
		// Check if there's a header, if not then add 128 bytes and write it
		char header[HEADER_LENGTH] = { 0 };

		file.seekg(HEADER_OFFSET, std::ios::end);
		file.read(header, HEADER_LENGTH);

		// Header isn't there so no tags written
		if (std::string(header) != HEADER) {
			char emptyChunk[128] = { 0 };
			file.seekp(0, std::ios::end);
			file.write(emptyChunk, 128);

			// Header
			writeData(HEADER_OFFSET, HEADER_LENGTH, HEADER);
		}

		// Title
		writeData(TITLE_OFFSET, TITLE_LENGTH, tags.title);

		// Artist
		writeData(ARTIST_OFFSET, ARTIST_LENGTH, tags.artist);

		// Album
		writeData(ALBUM_OFFSET, ALBUM_LENGTH, tags.album);

		// Year
		writeData(YEAR_OFFSET, YEAR_LENGTH, tags.year);

		// Comment
		writeData(COMMENT_OFFSET, COMMENT_LENGTH, tags.comment);

		// We can write the track number if comments is shorter than 29 bytes
		if (tags.comment.length() <= 28) {
			writeData(TRACK_OFFSET, TRACK_LENGTH, std::to_string(tags.track));
		}

		// Genre
		writeData(GENRE_OFFSET, GENRE_LENGTH, std::to_string(tags.genre));

		file.close();
		return true;
	};
private:
	ID3V1Tags tags;
	std::string filename;

	bool tagsRead = false;
	const std::string HEADER = "TAG";

	const int HEADER_OFFSET = -128;
	const int TITLE_OFFSET = -125;
	const int ARTIST_OFFSET = -95;
	const int ALBUM_OFFSET = -65;
	const int YEAR_OFFSET = -35;
	const int COMMENT_OFFSET = -31;
	const int ZERO_BYTE_OFFSET = -3;
	const int TRACK_OFFSET = -2;
	const int GENRE_OFFSET = -1;

	static const int HEADER_LENGTH = 3;
	static const int TITLE_LENGTH = 30;
	static const int ARTIST_LENGTH = 30;
	static const int ALBUM_LENGTH = 30;
	static const int YEAR_LENGTH = 4;
	static const int COMMENT_LENGTH = 30;
	static const int ZERO_BYTE_LENGTH = 1;
	static const int TRACK_LENGTH = 1;
	static const int GENRE_LENGTH = 1;
};