// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP

#include <vector>
#include "animators/Animator.hpp"

namespace ouzel
{
    namespace scene
    {
        class Sequence final: public Animator
        {
        public:
            explicit Sequence(const std::vector<Animator*>& initAnimators);
            explicit Sequence(const std::vector<std::unique_ptr<Animator>>& initAnimators);

            void play() override;

        protected:
            void updateProgress() override;

        private:
            Animator* currentAnimator = nullptr;
        };
    } // namespace scene
} // namespace ouzel

#endif // SEQUENCE_HPP
