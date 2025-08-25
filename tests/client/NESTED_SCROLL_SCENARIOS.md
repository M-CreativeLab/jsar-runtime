## Nested Scroll Container Behavior - Test Scenarios

This document describes test scenarios for validating the nested scroll container implementation.

### Scenario 1: Nested Scrollable Containers
```html
<div id="outer" style="overflow: auto; width: 200px; height: 200px;">
  <div id="inner" style="overflow: scroll; width: 100px; height: 100px;">
    <div style="width: 300px; height: 300px;">Large content</div>
  </div>
</div>
```

**Expected Behavior:**
1. Scroll events on the large content target the inner container first
2. When inner container reaches scroll boundary, events bubble to outer container
3. When outer container reaches boundary, no further scrolling occurs

### Scenario 2: Hidden Overflow Container
```html
<div id="outer" style="overflow: auto; width: 200px; height: 200px;">
  <div id="hidden" style="overflow: hidden; width: 100px; height: 100px;">
    <div style="width: 300px; height: 300px;">Large content</div>
  </div>
</div>
```

**Expected Behavior:**
1. Scroll events on large content skip the hidden overflow container
2. Events go directly to the outer auto container
3. Hidden container does not participate in scroll chain

### Scenario 3: Deep Nesting
```html
<div id="level1" style="overflow: auto;">
  <div id="level2" style="overflow: scroll;">
    <div id="level3" style="overflow: auto;">
      <div>Large content</div>
    </div>
  </div>
</div>
```

**Expected Behavior:**
1. Scroll chain: level3 → level2 → level1
2. Events bubble from deepest to shallowest when boundaries reached
3. Each container respects its own scroll boundaries

### Implementation Notes:
- DocumentEventDispatcher.buildScrollContainerChain() creates the scroll hierarchy
- ScrollableArea.canScrollInDirection() checks boundaries before scrolling
- Event bubbling occurs automatically when current container can't scroll further