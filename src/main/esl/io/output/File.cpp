/*
 * This file is part of ESL.
 * Copyright (C) 2020-2023 Sven Lukas
 *
 * ESL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ESL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser Public License for more details.
 *
 * You should have received a copy of the GNU Lesser Public License
 * along with ESL.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <esl/io/output/File.h>
#include <esl/Logger.h>

namespace esl {
inline namespace v1_6 {
namespace io {
namespace output {

namespace {
Logger logger("esl::io::output::File");
}

esl::io::Output File::create(const std::string& filename) {
	return esl::io::Output(std::unique_ptr<Reader>(new File(filename)));
}

File::File(const std::string& filename)
: file(new std::ifstream(filename))
{
	if(file->good()) {
		const auto begin = file->tellg();
		file->seekg(0, std::ios::end);
		const auto end = file->tellg();
		size = (end-begin);
	}
	else {
        logger.warn << "esl: cannot open file \"" << filename << "\"\n";
        file.reset();
	}
}

std::size_t File::read(void* buffer, std::size_t count) {
	if(!file) {
		return 0;
	}

	std::size_t remainingSize = getSizeReadable();
	if(count > remainingSize) {
		count = remainingSize;
	}

	if(count > 0) {
		file->read(static_cast<char*>(buffer), count);
	}

	return count;
}

std::size_t File::getSizeReadable() const {
	if(!file) {
		return 0;
	}

	return getSize() - pos;
}

bool File::hasSize() const {
	return file ? true : false;
//	return (bool) file;
}

std::size_t File::getSize() const {
	return size;
}

} /* namespace output */
} /* namespace io */
} /* inline namespace v1_6 */
} /* namespace esl */
