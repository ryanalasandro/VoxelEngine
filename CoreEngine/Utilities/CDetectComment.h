//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CDetectComment.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CDETECTCOMMENT_H
#define CDETECTCOMMENT_H

#include "CDSTrie.h"
#include "CCompilerUtil.h"

namespace Util
{
	enum COMMENT_TYPE
	{
		COMMENT_TYPE_NONE = 0x0,

		COMMENT_TYPE_SINGLE_OPEN = 0x1,
		COMMENT_TYPE_SINGLE_CLOSE = 0x2,
		COMMENT_TYPE_SINGLE = COMMENT_TYPE_SINGLE_OPEN | COMMENT_TYPE_SINGLE_CLOSE,

		COMMENT_TYPE_MULTI_OPEN = 0x4,
		COMMENT_TYPE_MULTI_CLOSE = 0x8,
		COMMENT_TYPE_MULTI = COMMENT_TYPE_MULTI_OPEN | COMMENT_TYPE_MULTI_CLOSE,

		COMMENT_TYPE_OPEN = COMMENT_TYPE_SINGLE_OPEN | COMMENT_TYPE_MULTI_OPEN,
		COMMENT_TYPE_CLOSE = COMMENT_TYPE_SINGLE_CLOSE | COMMENT_TYPE_MULTI_CLOSE,
	};

	const Util::CDSTrie<COMMENT_TYPE> COMMENT_TRIE = {
		{ "//", COMMENT_TYPE_SINGLE_OPEN },
		{ "\n", COMMENT_TYPE_SINGLE_CLOSE },
		{ "/*", COMMENT_TYPE_MULTI_OPEN },
		{ "*/", COMMENT_TYPE_MULTI_CLOSE },
	};

	// Simple class for detecting if we have entered into either a single or multi-lined comment in a c-style language.
	class CDetectComment
	{
	public:
		CDetectComment() :
			m_commentType(COMMENT_TYPE_NONE),
			m_commentNode(nullptr) {
		}

		// Method for keeping track of whether we're in a comment or not.
		COMMENT_TYPE InComment(char ch, CompilerWord& word)
		{
			m_commentNode = COMMENT_TRIE.Step(ch, m_commentNode);
			if(m_commentNode && m_commentNode->bTerminal)
			{
				if(m_commentType == COMMENT_TYPE_SINGLE)
				{
					if(m_commentNode->value == COMMENT_TYPE_SINGLE_CLOSE)
					{ // Has closed out a single-lined comment.
						m_commentType = COMMENT_TYPE_NONE;
					}

					m_commentNode = COMMENT_TRIE.GetHead();
					return COMMENT_TYPE_SINGLE;
				}
				else if(m_commentType == COMMENT_TYPE_MULTI)
				{
					if(m_commentNode->value == COMMENT_TYPE_MULTI_CLOSE)
					{ // Has closed out a multi-lined comment.
						m_commentType = COMMENT_TYPE_NONE;
					}

					m_commentNode = COMMENT_TRIE.GetHead();
					return COMMENT_TYPE_MULTI;
				}
				else
				{ // Check if we entered into a comment.
					if(m_commentNode->value == COMMENT_TYPE_SINGLE_OPEN)
					{
						m_commentType = COMMENT_TYPE_SINGLE;
						if(!word.Empty()) { word.Pop(); }

						m_commentNode = COMMENT_TRIE.GetHead();
						return COMMENT_TYPE_SINGLE_OPEN;

					}
					else if(m_commentNode->value == COMMENT_TYPE_MULTI_OPEN)
					{
						m_commentType = COMMENT_TYPE_MULTI;
						if(!word.Empty()) { word.Pop(); }

						m_commentNode = COMMENT_TRIE.GetHead();
						return COMMENT_TYPE_MULTI_OPEN;
					}
				}

				m_commentNode = COMMENT_TRIE.GetHead();
			}

			return m_commentType;
		}

	private:
		COMMENT_TYPE m_commentType;
		Util::CDSTrie<COMMENT_TYPE>::Node* m_commentNode;
	};
};

#endif
