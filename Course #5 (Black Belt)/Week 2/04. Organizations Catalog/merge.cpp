#include "yellow_pages.h"

#include <algorithm>
#include <tuple>

template <typename Message>
bool operator== (const Message& m1, const Message& m2) {
    return m1.SerializeAsString() == m2.SerializeAsString();
}

template <typename Message>
bool equals(const Message& m1, const Message& m2) {
    return m1.SerializeAsString() == m2.SerializeAsString();
}

namespace YellowPages {
    Company Merge(const Signals& signals, const Providers& providers) {
        Company result;

        {
            // address
            std::vector<Signal> max_prior_signals;
            size_t max_prior = 0;

            for (const auto& signal : signals) {
                if (signal.has_company() && signal.company().has_address()) {
                    const auto &prior = providers.at(signal.provider_id()).priority();

                    if (prior > max_prior) {
                        max_prior = prior;
                        max_prior_signals.clear();
                        max_prior_signals.push_back(signal);
                    } else if (prior == max_prior) {
                        max_prior_signals.push_back(signal);
                    }
                }
            }

            if (!max_prior_signals.empty()) {
                *result.mutable_address() = max_prior_signals.front().company().address();
            }
        }
        {
            // names
            std::vector<Signal> max_prior_signals;
            size_t max_prior = 0;

            for (const auto& signal : signals) {
                if (signal.has_company() && signal.company().names_size() > 0) {
                    const auto &prior = providers.at(signal.provider_id()).priority();

                    if (prior > max_prior) {
                        max_prior = prior;
                        max_prior_signals.clear();
                        max_prior_signals.push_back(signal);
                    } else if (prior == max_prior) {
                        max_prior_signals.push_back(signal);
                    }
                }
            }

            for (const auto& signal : max_prior_signals) {
                for (const auto& name : signal.company().names()) {
                    if (std::find_if(result.names().begin(), result.names().end(), [&name](const Name& item) {
                        return item == name;
                    }) == result.names().end()) {
                        *result.add_names() = name;
                    }
                }
            }
        }
        {
            // phones
            std::vector<Signal> max_prior_signals;
            size_t max_prior = 0;

            for (const auto& signal : signals) {
                if (signal.has_company() && signal.company().phones_size() > 0) {
                    const auto &prior = providers.at(signal.provider_id()).priority();

                    if (prior > max_prior) {
                        max_prior = prior;
                        max_prior_signals.clear();
                        max_prior_signals.push_back(signal);
                    } else if (prior == max_prior) {
                        max_prior_signals.push_back(signal);
                    }
                }
            }

            for (const auto& signal : max_prior_signals) {
                for (const auto& phone : signal.company().phones()) {
                    if (std::find_if(result.phones().begin(), result.phones().end(), [&phone](const Phone& item) {
                        return item == phone;
                    }) == result.phones().end()) {
                        *result.add_phones() = phone;
                    }
                }
            }
        }
        {
            // urls

            std::vector<Signal> max_prior_signals;
            size_t max_prior = 0;

            for (const auto& signal : signals) {
                if (signal.has_company() && signal.company().urls_size() > 0) {
                    const auto &prior = providers.at(signal.provider_id()).priority();

                    if (prior > max_prior) {
                        max_prior = prior;
                        max_prior_signals.clear();
                        max_prior_signals.push_back(signal);
                    } else if (prior == max_prior) {
                        max_prior_signals.push_back(signal);
                    }
                }
            }

            for (const auto& signal : max_prior_signals) {
                for (const auto& url : signal.company().urls()) {
                    if (std::find_if(result.urls().begin(), result.urls().end(), [&url](const Url& item) {
                        return item == url;
                    }) == result.urls().end()) {
                        *result.add_urls() = url;
                    }
                }
            }
        }
        {
            // working_time

            std::vector<Signal> max_prior_signals;
            size_t max_prior = 0;

            for (const auto& signal : signals) {
                if (signal.has_company() && signal.company().has_working_time()) {
                    const auto &prior = providers.at(signal.provider_id()).priority();

                    if (prior > max_prior) {
                        max_prior = prior;
                        max_prior_signals.clear();
                        max_prior_signals.push_back(signal);
                    } else if (prior == max_prior) {
                        max_prior_signals.push_back(signal);
                    }
                }
            }

            if (!max_prior_signals.empty()) {
                *result.mutable_working_time() = max_prior_signals.front().company().working_time();
            }
        }
        return result;
    }

}
