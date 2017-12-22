/*
 * Copyright (c) 2017 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.intel.podm.business.entities;

import com.intel.podm.business.entities.redfish.base.Entity;

import java.util.Random;

import static org.mockito.internal.util.reflection.Whitebox.setInternalState;

public final class BusinessEntitiesTestsHelper {
    private static final Random RANDOM = new Random(10);

    private BusinessEntitiesTestsHelper() {
    }

    public static <A extends Entity> A createEntity(Class<A> aClass) throws IllegalAccessException, InstantiationException {
        A instance = aClass.newInstance();
        setInternalState(instance, "id", RANDOM.nextLong());
        return instance;
    }
}