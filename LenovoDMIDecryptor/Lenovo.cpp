#include "Lenovo.hpp"

auto Lenovo::CompareNamespaceIds(PCNAMESPACE_ID Id1, PCNAMESPACE_ID Id2) -> bool
{
	return memcmp(Id1, Id2, sizeof(NAMESPACE_ID)) == 0;
};

auto Lenovo::IsSmbiosNamespace(PCNAMESPACE_ID Id) -> bool
{
	return CompareNamespaceIds(Id, &SMBIOS_NAMESPACE);
};

auto Lenovo::CompareEntryKeys(PCENTRY_KEY Key1, PCENTRY_KEY Key2) -> bool
{
	return CompareNamespaceIds(&Key1->NamespaceId, &Key2->NamespaceId) && Key1->Type == Key2->Type;
}