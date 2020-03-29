#pragma once

#include "ofMain.h"

namespace skinny {

class FileSelector : public ofThread
{
public:
	explicit FileSelector(const std::string& message, bool load = true);

	const std::filesystem::path& getPath() const;
	bool isLoading() const;

private:
	virtual void threadedFunction() override;

	const std::string message_;
	const bool load_;
	std::filesystem::path path_;

};

} // namespace skinny
