#include "Path.h"

#include "StringUtils.h"

#include <glib/gstdio.h>

#include <cstring>
#include <utility>

Path::Path(string path)
 : path(std::move(path))
{
}

Path::Path(const char* path)
 : path(path)
{
}

/**
 * @return true if empty
 */
bool Path::isEmpty() const
{
	return path.empty();
}

/**
 * Check if this is a file which exists
 */
bool Path::exists() const
{
	return g_file_test(path.c_str(), G_FILE_TEST_EXISTS);
}

/**
 * Delete the file
 *
 * @return true if the file is deleted or does not exists
 */
bool Path::deleteFile()
{
	if (!exists())
	{
		// Does not exists, therefore cannot be deleted
		return true;
	}

	int result = g_unlink(c_str());
	return result == 0;
}

/**
 * Compare the path with another one
 */
bool Path::operator==(const Path& other) const
{
	return this->path == other.path;
}

/**
 * Assign path
 */
Path& Path::operator=(string p)
{
	this->path = std::move(p);
	return *this;
}

/**
 * Assign path
 */
Path& Path::operator=(const char* p)
{
	this->path = p;
	return *this;
}

/**
 * @return true if this file has .xopp or .xoj extension
 */
bool Path::hasXournalFileExt() const
{
	return hasExtension(".xoj") || hasExtension(".xopp");
}

/**
 * Check if the path ends with this extension
 *
 * @param ext Extension, needs to be lowercase
 * @return true if the extension is there
 */
bool Path::hasExtension(const string& ext) const
{
	if (ext.length() > path.length())
	{
		return false;
	}

	string pathExt = path.substr(path.length() - ext.length());
	pathExt = StringUtils::toLowerCase(pathExt);

	return pathExt == ext;
}

/**
 * Clear the the last known extension (last .pdf, .pdf.xoj, .pdf.xopp etc.)
 */
void Path::clearExtensions()
{
	string plower = StringUtils::toLowerCase(path);
	auto rm_ext = [&](string const& ext) {
		if (StringUtils::endsWith(plower, ext))
		{
			this->path = path.substr(0, path.length() - ext.size());
		}
	};
	rm_ext(".xoj");
	rm_ext(".xopp");
}

/**
 * Return the Path as String
 */
const string Path::str() const
{
	return path;
}

/**
 * Return the Path as String
 */
const char* Path::c_str() const
{
	return path.c_str();
}

/**
 * Get escaped path, all " and \ are escaped
 */
string Path::getEscapedPath() const
{
	string escaped = path;
	StringUtils::replaceAllChars(escaped, {
		replace_pair('\\', "\\\\"),
		replace_pair('\"', "\\\"")
	});

	return escaped;
}

void Path::operator/=(const Path& p)
{
	*this /= p.str();
}

void Path::operator/=(const string& p)
{
	if (!path.empty())
	{
		char c = path.at(path.size() - 1);
		if (c != '/' && c != '\\')
		{
			path += G_DIR_SEPARATOR_S;
		}
	}
	path += p;
}

void Path::operator /=(const char* p)
{
	*this /= string(p);
}

Path Path::operator/(const Path& p) const
{
	return *this / p.c_str();
}

Path Path::operator/(const string& p) const
{
	return *this / p.c_str();
}

Path Path::operator/(const char* p) const
{
	Path ret(*this);
	ret /= p;
	return ret;
}

void Path::operator+=(const Path& p)
{
	path += p.str();
}

void Path::operator+=(const string& p)
{
	path += p;
}

void Path::operator +=(const char* p)
{
	path += p;
}

/**
 * Return the Filename of the path
 */
string Path::getFilename() const
{
	size_t separator = path.find_last_of("/\\");

	if (separator == string::npos)
	{
		return str();
	}

	return path.substr(separator + 1);
}

/**
 * Convert this path to Uri
 */
string Path::toUri(GError** error)
{
	char* uri = g_filename_to_uri(path.c_str(), nullptr, error);

	if (uri == nullptr)
	{
		return "";
	}

	string uriString = uri;
	g_free(uri);
	return uriString;
}

#ifndef BUILD_THUMBNAILER
/**
 * Convert this path to GFile
 */
GFile* Path::toGFile()
{
	return g_file_new_for_path(path.c_str());
}
#endif

/**
 * Get the parent path
 */
Path Path::getParentPath() const
{
	size_t separator = path.find_last_of("/\\");

	if (separator == string::npos)
	{
		return {};
	}

	return Path{path.substr(0, separator)};
}

/**
 * Convert an uri to a path, if the uri does not start with file:// an empty Path is returned
 */
Path Path::fromUri(const string& uri)
{
	if (!StringUtils::startsWith(uri, "file://"))
	{
		return Path();
	}

	gchar* filename = g_filename_from_uri(uri.c_str(), nullptr, nullptr);
	Path p(filename);
	g_free(filename);

	return p;
}

#ifndef BUILD_THUMBNAILER
Path Path::fromGFile(GFile* file)
{
	char* uri = g_file_get_uri(file);
	string sUri = uri;
	g_free(uri);

	return fromUri(sUri);
}
#endif
