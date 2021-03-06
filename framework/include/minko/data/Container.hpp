/*
Copyright (c) 2014 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "minko/Common.hpp"
#include "minko/data/Provider.hpp"

namespace minko
{
    namespace data
    {
        class Container :
            public std::enable_shared_from_this<Container>
        {

        public:
            typedef std::shared_ptr<Container>                              Ptr;
            typedef Signal<Ptr, const std::string&>                         PropertyChangedSignal;

        private:
            typedef std::shared_ptr<PropertyChangedSignal>                  PropertyChangedSignalPtr;

            typedef std::shared_ptr<Provider>                               ProviderPtr;
            typedef std::shared_ptr<data::AbstractFilter>                   AbsFilterPtr;
            typedef Signal<ProviderPtr, const std::string&>                 ProviderPropertyChangedSignal;
            typedef ProviderPropertyChangedSignal::Slot                     ProviderPropertyChangedSlot;

            std::list<ProviderPtr>                                          _providers;
            std::unordered_map<std::string, ProviderPtr>                    _propertyNameToProvider;
            std::unordered_map<ProviderPtr, uint>                           _providersToNumUse;
            std::unordered_map<ProviderPtr, uint>                           _providerToIndex;

            std::shared_ptr<Provider>                                       _arrayLengths;

            PropertyChangedSignalPtr                                        _propertyAdded;
            PropertyChangedSignalPtr                                        _propertyRemoved;
            std::unordered_map<std::string, PropertyChangedSignalPtr>       _propValueChanged;
            std::unordered_map<std::string, PropertyChangedSignalPtr>       _propReferenceChanged;

            std::unordered_map<ProviderPtr, std::list<Any>>                 _propertyAddedOrRemovedSlots;
            std::unordered_map<ProviderPtr, ProviderPropertyChangedSlot>    _providerValueChangedSlot;
            std::unordered_map<ProviderPtr, ProviderPropertyChangedSlot>    _providerReferenceChangedSlot;

            Signal<Ptr, ProviderPtr>::Ptr                                   _providerAdded;
            Signal<Ptr, ProviderPtr>::Ptr                                   _providerRemoved;

            static uint                                                     CONTAINER_ID;


        public:
            uint _containerId;
            static
            Ptr
            create()
            {
                auto container = std::shared_ptr<Container>(new Container());

                container->initialize();

                return container;
            }

            void
            initialize();

            void
            addProvider(std::shared_ptr<Provider> provider);

            void
            addProvider(std::shared_ptr<ArrayProvider> provider);

            void
            removeProvider(std::shared_ptr<Provider> provider);

            void
            removeProvider(std::shared_ptr<ArrayProvider> provider);

            bool
            hasProvider(std::shared_ptr<Provider> provider) const;

            bool
            hasProperty(const std::string&) const;

            bool
            isLengthProperty(const std::string&) const;

            inline
            int
            getProviderIndex(ProviderPtr provider) const
            {
                auto foundIndexIt = _providerToIndex.find(provider);

                return foundIndexIt != _providerToIndex.end() ? foundIndexIt->second : -1;
            }

            template <typename T>
            T
            get(const std::string& propertyName) const
            {
                assertPropertyExists(propertyName);

                const auto& provider = _propertyNameToProvider.find(propertyName)->second;
                auto unformatedPropertyName = unformatPropertyName(provider, propertyName);

                return provider->get<T>(unformatedPropertyName, true);
            }

            template <typename T>
            void
            set(const std::string& propertyName, T value)
            {
                assertPropertyExists(propertyName);

                auto provider = _propertyNameToProvider[propertyName];
                auto unformatedPropertyName = unformatPropertyName(provider, propertyName);

                provider->set<T>(unformatedPropertyName, value);
            }

            template <typename T>
            bool
            propertyHasType(const std::string& propertyName, bool skipPropertyNameFormatting = false) const
            {
                assertPropertyExists(propertyName);

                const auto& provider = _propertyNameToProvider.find(propertyName)->second;

                auto unformatedPropertyName = unformatPropertyName(provider, propertyName);

                return provider->propertyHasType<T>(unformatedPropertyName, skipPropertyNameFormatting);
            }

            inline
            PropertyChangedSignalPtr
            propertyAdded() const
            {
                return _propertyAdded;
            }

            inline
            PropertyChangedSignalPtr
            propertyRemoved() const
            {
                return _propertyRemoved;
            }

            PropertyChangedSignalPtr
            propertyValueChanged(const std::string& propertyName);

            PropertyChangedSignalPtr
            propertyReferenceChanged(const std::string& propertyName);

            inline
            Signal<Ptr, Provider::Ptr>::Ptr
            providerAdded() const
            {
                return _providerAdded;
            }

            inline
            Signal<Ptr, Provider::Ptr>::Ptr
            providerRemoved() const
            {
                return _providerRemoved;
            }

            inline
            const std::list<ProviderPtr>&
            providers() const
            {
                return _providers;
            }

            inline
            const std::vector<std::string>
            properties() const
            {
                std::vector<std::string> properties;

                for (auto& kv : _propertyNameToProvider)
                    properties.push_back(kv.first);

                return properties;
            }

            Ptr
            filter(const std::set<AbsFilterPtr>&, Ptr = nullptr) const;

        private:
            Container();

            void
            assertPropertyExists(const std::string& propertyName) const;

            void
            providerPropertyAddedHandler(ProviderPtr, const std::string& propertyName);

            void
            providerPropertyRemovedHandler(ProviderPtr, const std::string& propertyName);

            void
            providerValueChangedHandler(ProviderPtr, const std::string& propertyName);

            void
            providerReferenceChangedHandler(ProviderPtr, const std::string& propertyName);

            std::string
            formatPropertyName(ProviderPtr  arrayProvider, const std::string&) const;

            std::string
            unformatPropertyName(ProviderPtr  arrayProvider, const std::string&) const;

            inline
            void
            assertProviderDoesNotExist(std::shared_ptr<Provider> provider) const
            {
#ifdef DEBUG
                if (std::find(_providers.begin(), _providers.end(), provider) != _providers.end())
                    throw std::invalid_argument("provider");
#endif // DEBUG
            }

            inline
            void
            assertProviderExists(std::shared_ptr<Provider> provider) const
            {
#ifdef DEBUG
                if (std::find(_providers.begin(), _providers.end(), provider) == _providers.end())
                    throw std::invalid_argument("provider");
#endif // DEBUG
            }
        };
    }
}
