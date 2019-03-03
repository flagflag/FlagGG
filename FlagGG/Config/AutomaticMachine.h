#pragma once

#include "Container/Str.h"

#include <string>
#include <vector>
#include <stdint.h>

namespace FlagGG
{
	namespace Config
	{
#define AM_MID_SIGN '='

		typedef uint8_t Byte;

		static const uint32_t CHAR_COUNT = 256;

		class StateNode;
		typedef StateNode* StatePtr;

		typedef struct StateNode
		{
			StateNode();

			StatePtr next[CHAR_COUNT];
		};

		class AutomaticMachine
		{
		public:
			virtual ~AutomaticMachine() = default;

			virtual bool Accapt(const Byte* bufferStart, const Byte* bufferEnd, const Byte*& index, Container::String& token) = 0;

		protected:
			void LinkSet(StatePtr state1, StatePtr state2, const char* set);

			void LinkNoneSet(StatePtr state1, StatePtr state2, const char* set);

			void LinkNoneSet(StatePtr state, const char* set);
		};

		typedef AutomaticMachine* AutomaticMachinePtr;

		class EmptyAutomaticMachine : public AutomaticMachine
		{
		public:
			EmptyAutomaticMachine();

			~EmptyAutomaticMachine() override = default;

			bool Accapt(const Byte* bufferStart, const Byte* bufferEnd, const Byte*& index, Container::String& token) override;

		private:
			StateNode pool_[1];

			StatePtr start_;
		};

		class CommentAutomationMachine : public EmptyAutomaticMachine
		{
		public:
			CommentAutomationMachine() = default;

			~CommentAutomationMachine() override = default;

			bool Accapt(const Byte* bufferStart, const Byte* bufferEnd, const Byte*& index, Container::String& token) override;
		};

		class TypeKeywordAutomaticMachine : public AutomaticMachine
		{
		public:
			TypeKeywordAutomaticMachine();

			~TypeKeywordAutomaticMachine() override = default;

			bool Accapt(const Byte* bufferStart, const Byte* bufferEnd, const Byte*& index, Container::String& token) override;

		private:
			StateNode pool_[3];

			StatePtr start_;
			StatePtr token_;
			StatePtr end_;

			std::vector<Container::String> keywords_;
		};

		class VariableAutomaticMachine : public AutomaticMachine
		{
		public:
			VariableAutomaticMachine();

			~VariableAutomaticMachine() override = default;

			bool Accapt(const Byte* bufferStart, const Byte* bufferEnd, const Byte*& index, Container::String& token) override;

		private:
			StateNode pool_[6];

			StatePtr start_;
			StatePtr end_;
			StatePtr token_;
			StatePtr quotationStart_;
			StatePtr quotationEnd_;
			StatePtr escapes_;
		};

		class ContentAutomaticMachine : public AutomaticMachine
		{
		public:
			ContentAutomaticMachine();

			~ContentAutomaticMachine() override = default;

			bool Accapt(const Byte* bufferStart, const Byte* bufferEnd, const Byte*& index, Container::String& token) override;

		private:
			StateNode pool_[11];

			StatePtr start_;
			StatePtr end_;
			StatePtr token_;
			StatePtr quotationStart_;
			StatePtr quotationEnd_;
			StatePtr escapes_;

			StatePtr leftBracket_;
			StatePtr rightBracket_;
			StatePtr textStart_;
			StatePtr textEnd_;
			StatePtr textEscapes_;
		};
	}
}


