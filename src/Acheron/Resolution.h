#pragma once

namespace Acheron
{
	class EventData
	{
	public:
		static constexpr const char* DEFAULT_NAME = "UNTITLED";

		struct CONDITION_DATA
		{
			enum class ConditionType : uint8_t
			{
				Race,
				Faction,
				Keyword,
				Location,
				WorldSpace,
				QuestDone,
				QuestRunning,
			};

			union Condition
			{
				const char* racetype;
				RE::TESFaction* faction;
				RE::TESQuest* quest;
				RE::BGSKeyword* keyword;
				RE::BGSLocation* location;
				RE::TESWorldSpace* worldspace;
			};

		public:
			CONDITION_DATA(ConditionType a_type, std::string a_conditionobject, bool a_compare);
			~CONDITION_DATA() = default;

			_NODISCARD bool Check(RE::Actor* a_target) const;

		private:
			ConditionType type;
			Condition value;
			bool compare;
		};

		enum ConditionTarget
		{
			Assailant = 0,
			Victim = 1,
			Unspecified = 2,	// IDEA: implement for things like quest stage conditioning
			Total
		};

		enum class Flags : uint8_t
		{
			Teleport = 1 << 0,	// Does the event teleport the victim away
			InCombat = 1 << 1,	// Can the event start mid combat
			Hidden = 1 << 2,		// Is the event hidden from the player, i.e. no settings displayed
		};

		enum Priority : uint8_t
		{
			Default = 0,
			Common = 1,
			StoryGeneric = 2,
			StoryPriority = 3,
			p_Total
		};

		/// @brief Construct a new quest object from a given file
		/// @param a_file The yaml file to construct the quest from
		/// @throw ParseException when the given file has errors or requirements arent met
		EventData(const std::string& a_file);
		EventData(RE::TESQuest* a_quest) :
				quest(a_quest) { assert(a_quest); };
		~EventData() = default;

		/// @brief Validate that this quest is allowed to play for the given actors
		/// @param a_victoires the actors which defeated the victim
		/// @param a_victim the actor to validate the conditions from
		/// @return if all conditions evaluate to true for some valid match
		bool CheckConditions(const std::vector<RE::Actor*>& a_victoires, RE::Actor* a_victim) const;

	public:
		RE::TESQuest* quest = nullptr;
		std::string name = DEFAULT_NAME;

		uint16_t cooldown{ 0 };
		Priority priority{ Priority::Default };
		uint8_t weight{ 50 };

		stl::enumeration<Flags, uint8_t> flags{ Flags::Teleport };
		std::vector<CONDITION_DATA> conditions[ConditionTarget::Total];
	};

	class Resolution
	{
	public:
		enum Type
		{
			Hostile = 0,		// When the player lost against a hostile actor
			Follower = 1,		// When the player lost but a follower was victorious
			Civilian = 2,		// When the player lost but a non hostile civilian was victorious
			Guard = 3,			// When the player lost against a guard
			NPC = 4,				// When the player was not involved in the encounter

			Total
		};

	public:
		static void Initialize();
		static void Save();

		/// @brief Lookup a consequence event for the given victim
		/// @param type The type of event to look for
		/// @param a_victim The victim to look up an event for
		/// @param a_victoires The actors which defeated the victim
		/// @param a_incombat If combat is still ongoing
		/// @return An event for the described environment; nullptr if none can be found
		static RE::TESQuest* SelectQuest(Type type, RE::Actor* a_victim, const std::vector<RE::Actor*>& a_victoires, bool a_incombat);

		/// @brief Retrieve all existing current registered events of a given type
		/// @param a_type The event type to retrieve events for
		/// @return vector of pairs <name, weight> for all events of the given type
		static std::vector<std::pair<const std::string&, uint8_t>> GetEvents(Type a_type);

		/// @brief Manipulate the weight of a given event belonging to the respective type
		/// @param a_name The name of the event
		/// @param a_type The type the event belongs to
		/// @param a_weight The new weight of the event
		static void SetEventWeight(const std::string& a_name, Type a_type, uint8_t a_weight);

	private:
		static inline std::vector<EventData> Events[Type::Total];
	};
}