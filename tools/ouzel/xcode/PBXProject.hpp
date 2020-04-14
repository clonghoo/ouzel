// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXPROJECT_HPP
#define OUZEL_XCODE_PBXPROJECT_HPP

#include <map>
#include <vector>
#include "PBXObject.hpp"
#include "PBXGroup.hpp"
#include "XCConfigurationList.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PBXProject final: public PBXObject
        {
        public:
            PBXProject() = default;

            std::string getIsa() const override { return "PBXProject"; }

            plist::Value encode() const override
            {
                auto result = PBXObject::encode();

                result["attributes"] = plist::Value::Dictionary{
                    {"LastUpgradeCheck", "0800"},
                    {"ORGANIZATIONNAME", organization}
                };
                if (buildConfigurationList)
                    result["buildConfigurationList"] = toString(buildConfigurationList->getId());
                result["compatibilityVersion"] = "Xcode 9.3";
                result["developmentRegion"] = "en";
                result["hasScannedForEncodings"] = 0;
                result["knownRegions"] = plist::Value::Array{"en", "Base"};
                if (mainGroup) result["mainGroup"] = toString(mainGroup->getId());
                if (productRefGroup)
                    result["productRefGroup"] = toString(productRefGroup->getId());
                result["projectDirPath"] = "";
                result["projectRoot"] = "";
                result["targets"] = plist::Value::Array{};
                for (auto target : targets)
                    if (target) result["targets"].pushBack(toString(target->getId()));

                if (!projectReferences.empty())
                {
                    auto references = plist::Value::Dictionary{};

                    for (const auto& projectReference : projectReferences)
                    {
                        auto object = projectReference.second;
                        if (object) references[projectReference.first] = toString(object->getId());
                    }

                    result["projectReferences"] = plist::Value::Array{references};
                }

                return result;
            }

            std::string organization;
            const XCConfigurationList* buildConfigurationList = nullptr;
            const PBXGroup* mainGroup = nullptr;
            const PBXGroup* productRefGroup = nullptr;
            std::map<std::string, const PBXObject*> projectReferences;
            std::vector<const PBXTarget*> targets;
        };
    }
}

#endif // OUZEL_XCODE_PBXPROJECT_HPP