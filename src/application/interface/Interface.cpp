#include "src/application/Application.h"

#include "src/application/interface/Interface.h"
#include "src/application/interface/components/ComponentManager.h"
#include "src/application/interface/components/types/Component.h"

// creates components every frame, but updates content of the screen
// only when a workflow change occurs. this allows for immediate
// processing of any commands while creating the component. still
// offers a decent amount of performance with deferred rendering.
void Interface::immediateProcess() {
  bool render = false;
  // if workflow changed, trigger render
  Workflow &workflow = app->workflow();
  if (workflow.hasChanges()) {
    Serial.println("Something changed, re-rendering.");
    workflow.applyChanges();
    render = true;
  }
  // renderable component must always be deleted after using
  manager->createComponent(workflow.getState());
  if(render) {
    manager->renderComponent();
  }
  manager->deleteComponent();
}

// creates component and renders only when a workflow change is detected.
// with this paradigm components live longer, so you must rely on
// binding input event listeners instead of performing input checks in
// your component creation code. theoretically more efficient than
// immediate processing because component code does not need to be run
// every frame. however, this does complicate setting up event handling
// code somewhat.
void Interface::deferredProcess() {
  Workflow &workflow = app->workflow();
  // if workflow changed, trigger render
  if (workflow.hasChanges()) {
    workflow.applyChanges();
    Serial.println("Something changed, re-rendering.");
    // create component and render.
    // if a component already exists, it will be disposed first
    manager->createComponent(workflow.getState());
    manager->renderComponent();
  }
}