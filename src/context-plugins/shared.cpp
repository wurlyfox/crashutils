typedef struct
{
  void *field;
  int id;
  contextField *next;
} contextField;

typedef struct
{
  context *ctxt;
  contextField *fields;
} contextMain;

class context_plugin_shared : public context_plugin
{
  private:
  
  static contextMain contextList[0x10];
  
  public:
  
  bool getShared(void **field)
  {
    int lp;
    
    // go through each context in the list...
    for (lp = 0; lp < 0x10; lp++)
    {
      // ...while that position in the list is taken by a context
      if (contextList[lp])
      {
        contextMain *main = contextList[lp]; 
        
        // if the context associated with this plugin is a main context as determined
        // by iterating through the list, then the plugin needs to create the object 
        // for this field, since it is not yet recorded for its associated main context
        if (main->ctxt == ctxt)  
          return false; 
        else
        {
          // otherwise, if the context associated with the plugin shares geometry with
          // the main context, then it is one of the shared contexts of this main context
          if (main->ctxt->contextScene->geom == this->ctxt->contextScene->geom)
          {
            // we can calculate the field id based on its offset; all fields of plugins
            // of this class type will have the same offsets
            contextField *mainField = main->fields;
            int id = (unsigned long)*field - (unsigned long)this;  
            
            // find if the field has already been recorded by the main context
            while (mainField->id != id)
            {
              mainField = mainField->next;
              if (mainField == 0)
              {
                *field = 0;
                return true;
              }
            }
            
            // if so then return it
            *field = mainField->field;
            return true;
          }
        }
      }
      else
        break;
    }
    
    // this is a main context 
    // record it in the next open position in the list
    if (lp < 0x10)
    {
      contextList[lp] = new contextMain;
      contextList[lp]->main = this;
      contextList[lp]->fields = 0;
    }
      
    return false;
  }
  
  bool makeShared(void **field, void *data)
  {
    int lp;
    
    // go through each context in the list...
    for (lp = 0; lp < 0x10; lp++)
    {
      contextMain *main;
      
      // ...if that position in the list is not taken by a context
      if (!contextList[lp])
      {
        contextList[lp] = new contextMain;
        
        main            = contextList[lp];
        main->ctxt      = this;
        main->fields    = 0;
      }
      else
      {
        main = contextList[lp];
      }
        
      // if the plugin associated with this context indeed shares the same
      // geometry as the main context
      if (main->ctxt->contextScene->geom == this->ctxt->contextScene->geom)
      {
        // calculate the desired field's offset (ID)
        contextField *mainField = main->fields;
        int id = (unsigned long)*field - (unsigned long)this; 

        // while fields exist, iterate through the shared field list to 
        // check if the desired field has already been recorded
        if (!mainField)
        {
          main->fields     = new contextField;
          mainField        = main->fields;
          mainField->next  = 0;
          mainField->id    = id;
          mainField->field = data; 
          *field = mainField->field;
        }
        else if (mainField->id == id)
          *field = mainField->field; 
        else
        {            
          contextField *prevField = mainField;
          contextField *nextField = prevField->next;
          while (nextField)
          {
            if (nextField->id == id)
            {
              *field = nextField->field;
              break;
            }
            else
            {
              prevField = nextField;
              nextField = prevField->next;
            }
          }
          
          prevField->next  = new contextField;
          nextField        = prevField->next;
          nextField->id    = id;
          nextField->field = data;
          nextField->next  = 0;          
          *field           = nextField->field;
        }
        
        return true;
      }
      else
        return false; //error
    }
       
    return false;
  }
}