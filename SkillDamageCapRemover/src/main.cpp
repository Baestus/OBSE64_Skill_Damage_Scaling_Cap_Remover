#include "PCH.h" // Include Precompiled Header first

namespace Hooks
{
    // Function specifically for patching the strength cap CMOV instruction
    void DamageFormulaSkillCapRemover()
    {
        // --- Configuration ---
        // Offset of the CMOV instruction (0F 4F D8) from the Oblivion.exe base address.
        // Example: If Ghidra shows base 140000000 and instruction at 14684a835,
        // the offset for the Luck if is 1468CA02B - 140000000 = 0x68CA02B.
        constexpr REL::Offset instructionOffset(0x68CA02B);

        // Size of the original instruction being replaced (CMOVBE RAX, RCX is 4 bytes)
        constexpr size_t instructionSize = 4;

        // Get the relocation object (calculates the address in memory)
        REL::Relocation<std::uintptr_t> target(instructionOffset);

        constexpr std::array<std::uint8_t, instructionSize> patchBytes = {
            0x48, 0x89, 0xC8, // MOV RAX, RCX
            0x90              // NOP
        };

        // Write NOP instructions (0x90) over the original instruction bytes
        // This effectively disables the conditional move that caps strength at 100.
        target.write(patchBytes.data(), patchBytes.size());
    }

    // Main function to install all hooks/patches
    void Install()
    {
        // Call the specific patch function(s)
        DamageFormulaSkillCapRemover();

    }

}

namespace // Anonymous namespace for local functions
{
    // OBSE Message Listener
    void MessageHandler(OBSE::MessagingInterface::Message* a_msg)
    {
        switch (a_msg->type) {
            // kPostLoad is sent after OBSE and all plugins are loaded,
            // making it a safe time to apply patches.
            case OBSE::MessagingInterface::kPostLoad:
                Hooks::Install();
                break;
            default:
                break;
        }
    }
}

// OBSE Plugin Entry Point (used by CommonLibOB64's rule)
OBSE_PLUGIN_LOAD(OBSE::LoadInterface* a_obse)
{
    OBSE::Init(a_obse);

    // Get the OBSE messaging interface and register our listener
    auto* messaging = OBSE::GetMessagingInterface();
    if (messaging) {
        messaging->RegisterListener(MessageHandler);
    } else {
        return false;
    }

    return true;
}
