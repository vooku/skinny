#pragma once

#include "ofMain.h"

namespace skinny {

class VideoSelector : public ofThread
{
public:
	using IndexType = unsigned int;

	struct Context {
		explicit Context(IndexType index);

		const IndexType index;
		std::filesystem::path path;
	};

	explicit VideoSelector(IndexType index);

	const Context& getContext() const;

private:
	virtual void threadedFunction() override;

	Context ctx_;
};

} // namespace skinny
