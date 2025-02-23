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

#include <esl/io/FilePosition.h>

namespace esl {
inline namespace v1_6 {
namespace io {

FilePosition::FilePosition(const std::string& aFileName, int aLineNo)
: fileName(aFileName),
  lineNo(aLineNo)
{ }

const std::string& FilePosition::getFileName() const noexcept {
	return fileName;
}

const int FilePosition::getLineNo() const noexcept {
	return lineNo;
}

} /* namespace io */
} /* inline namespace v1_6 */
} /* namespace esl */
