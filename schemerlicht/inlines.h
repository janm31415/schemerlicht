if ((b) == 2)
  {
  const schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, (a)+(c)+1, schemerlicht_object);
  switch (schemerlicht_object_get_type(arg1))
    {
    case schemerlicht_object_type_closure:
    {
    const schemerlicht_object* arg2 = schemerlicht_vector_at(&ctxt->stack, (a)+(c)+2, schemerlicht_object);
    switch (schemerlicht_object_get_type(arg2))
      {
      case schemerlicht_object_type_fixnum:
      {
      switch (function_id)
        {
        case SCHEMERLICHT_CLOSUREREF:
        {
        schemerlicht_set_object(target, schemerlicht_vector_at(&arg1->value.v, arg2->value.fx, schemerlicht_object));
        continue;
        }
        default:
          goto slow_lane;          
        }
      }
      default:
        goto slow_lane;        
      }
    }
    case schemerlicht_object_type_fixnum:
    {
    const schemerlicht_object* arg2 = schemerlicht_vector_at(&ctxt->stack, (a)+(c)+2, schemerlicht_object);
    switch (schemerlicht_object_get_type(arg2))
      {
      case schemerlicht_object_type_fixnum:
      {
      switch (function_id)
        {
        case SCHEMERLICHT_ADD:
        case SCHEMERLICHT_FXADD:
        {
        target->value.fx = arg1->value.fx + arg2->value.fx;
        target->type = schemerlicht_object_type_fixnum;
        continue;
        }
        case SCHEMERLICHT_SUB:
        case SCHEMERLICHT_FXSUB:
        {
        target->value.fx = arg1->value.fx - arg2->value.fx;
        target->type = schemerlicht_object_type_fixnum;
        continue;
        }
        case SCHEMERLICHT_MUL:
        case SCHEMERLICHT_FXMUL:
        {
        target->value.fx = arg1->value.fx * arg2->value.fx;
        target->type = schemerlicht_object_type_fixnum;
        continue;
        }
        case SCHEMERLICHT_DIV:
        case SCHEMERLICHT_FXDIV:
        {
        if (arg2->value.fx == 0)
          target->type = schemerlicht_object_type_undefined;
        else
          {
          target->value.fx = arg1->value.fx / arg2->value.fx;
          target->type = schemerlicht_object_type_fixnum;
          }
        continue;
        }
        case SCHEMERLICHT_NOT_EQUAL:
        {
        target->type = (arg1->value.fx != arg2->value.fx) ? schemerlicht_object_type_true : schemerlicht_object_type_false;
        continue;
        }
        case SCHEMERLICHT_EQUAL:
        case SCHEMERLICHT_FXEQUAL:
        {
        target->type = (arg1->value.fx == arg2->value.fx) ? schemerlicht_object_type_true : schemerlicht_object_type_false;
        continue;
        }
        case SCHEMERLICHT_LESS:
        case SCHEMERLICHT_FXLESS:
        {
        target->type = (arg1->value.fx < arg2->value.fx) ? schemerlicht_object_type_true : schemerlicht_object_type_false;
        continue;
        }
        case SCHEMERLICHT_GREATER:
        case SCHEMERLICHT_FXGREATER:
        {
        target->type = (arg1->value.fx > arg2->value.fx) ? schemerlicht_object_type_true : schemerlicht_object_type_false;
        continue;
        }
        case SCHEMERLICHT_LEQ:
        case SCHEMERLICHT_FXLEQ:
        {
        target->type = (arg1->value.fx <= arg2->value.fx) ? schemerlicht_object_type_true : schemerlicht_object_type_false;
        continue;
        }
        case SCHEMERLICHT_GEQ:
        case SCHEMERLICHT_FXGEQ:
        {
        target->type = (arg1->value.fx >= arg2->value.fx) ? schemerlicht_object_type_true : schemerlicht_object_type_false;
        continue;
        }
        default:
          goto slow_lane;          
        }
      }
      default:
        goto slow_lane;        
      }
    }
    case schemerlicht_object_type_flonum:
    {
    const schemerlicht_object* arg2 = schemerlicht_vector_at(&ctxt->stack, (a)+(c)+2, schemerlicht_object);
    switch (schemerlicht_object_get_type(arg2))
      {
      case schemerlicht_object_type_flonum:
      {
      switch (function_id)
        {
        case SCHEMERLICHT_ADD:
        case SCHEMERLICHT_FLADD:
        {
        target->value.fl = arg1->value.fl + arg2->value.fl;
        target->type = schemerlicht_object_type_flonum;
        continue;
        }
        case SCHEMERLICHT_SUB:
        case SCHEMERLICHT_FLSUB:
        {
        target->value.fl = arg1->value.fl - arg2->value.fl;
        target->type = schemerlicht_object_type_flonum;
        continue;
        }
        case SCHEMERLICHT_MUL:
        case SCHEMERLICHT_FLMUL:
        {
        target->value.fl = arg1->value.fl * arg2->value.fl;
        target->type = schemerlicht_object_type_flonum;
        continue;
        }
        case SCHEMERLICHT_DIV:
        case SCHEMERLICHT_FLDIV:
        {
        target->value.fl = arg1->value.fl / arg2->value.fl;
        target->type = schemerlicht_object_type_flonum;
        continue;
        }
        case SCHEMERLICHT_NOT_EQUAL:
        {
        target->type = (arg1->value.fl != arg2->value.fl) ? schemerlicht_object_type_true : schemerlicht_object_type_false;
        continue;
        }
        case SCHEMERLICHT_EQUAL:
        case SCHEMERLICHT_FLEQUAL:
        {
        target->type = (arg1->value.fl == arg2->value.fl) ? schemerlicht_object_type_true : schemerlicht_object_type_false;
        continue;
        }
        case SCHEMERLICHT_LESS:
        case SCHEMERLICHT_FLLESS:
        {
        target->type = (arg1->value.fl < arg2->value.fl) ? schemerlicht_object_type_true : schemerlicht_object_type_false;
        continue;
        }
        case SCHEMERLICHT_GREATER:
        case SCHEMERLICHT_FLGREATER:
        {
        target->type = (arg1->value.fl > arg2->value.fl) ? schemerlicht_object_type_true : schemerlicht_object_type_false;
        continue;
        }
        case SCHEMERLICHT_LEQ:
        case SCHEMERLICHT_FLLEQ:
        {
        target->type = (arg1->value.fl <= arg2->value.fl) ? schemerlicht_object_type_true : schemerlicht_object_type_false;
        continue;
        }
        case SCHEMERLICHT_GEQ:
        case SCHEMERLICHT_FLGEQ:
        {
        target->type = (arg1->value.fl >= arg2->value.fl) ? schemerlicht_object_type_true : schemerlicht_object_type_false;
        continue;
        }
        default:
          goto slow_lane;          
        }
      }
      default:
        goto slow_lane;        
      }
    }    
    case schemerlicht_object_type_vector:
    {
    const schemerlicht_object* arg2 = schemerlicht_vector_at(&ctxt->stack, (a)+(c)+2, schemerlicht_object);
    switch (schemerlicht_object_get_type(arg2))
      {
      case schemerlicht_object_type_fixnum:
      {
      switch (function_id)
        {
        case SCHEMERLICHT_VECTORREF:
        {
        if (0 <= arg2->value.fx && arg2->value.fx < arg1->value.v.vector_size)
          {
          schemerlicht_set_object(target, schemerlicht_vector_at(&arg1->value.v, arg2->value.fx, schemerlicht_object));
          }
        else
          {
          target->type = schemerlicht_object_type_undefined;
          }
        continue;
        }
        default:
          goto slow_lane;          
        }
      }
      default:
        goto slow_lane;        
      }
    }
    case schemerlicht_object_type_pair:
    {
    switch (function_id)
      {
      case SCHEMERLICHT_SET_CAR:
      {
      const schemerlicht_object* arg2 = schemerlicht_vector_at(&ctxt->stack, (a)+(c)+2, schemerlicht_object);
      schemerlicht_set_object(schemerlicht_vector_at(&arg1->value.v, 0, schemerlicht_object), arg2);
      continue;
      }
      case SCHEMERLICHT_SET_CDR:
      {
      const schemerlicht_object* arg2 = schemerlicht_vector_at(&ctxt->stack, (a)+(c)+2, schemerlicht_object);
      schemerlicht_set_object(schemerlicht_vector_at(&arg1->value.v, 1, schemerlicht_object), arg2);
      continue;
      }
      default:
        goto slow_lane;        
      }
    }
    default:
      goto slow_lane;      
    }
  }
else if ((b) == 1)
  {
  const schemerlicht_object* arg1 = schemerlicht_vector_at(&ctxt->stack, (a)+(c)+1, schemerlicht_object);
  switch (schemerlicht_object_get_type(arg1))
    {
    case schemerlicht_object_type_pair:
    {
    switch (function_id)
      {
      case SCHEMERLICHT_CAR:
        schemerlicht_set_object(target, schemerlicht_vector_at(&arg1->value.v, 0, schemerlicht_object));
        continue;
      case SCHEMERLICHT_CDR:
        schemerlicht_set_object(target, schemerlicht_vector_at(&arg1->value.v, 1, schemerlicht_object));
        continue;
      default:
        goto slow_lane;        
      }
    }
    case schemerlicht_object_type_flonum:
    {
    switch (function_id)
      {
      case SCHEMERLICHT_FLOOR:
        target->type = schemerlicht_object_type_flonum;
        target->value.fl = floor(arg1->value.fl);
        continue;
      case SCHEMERLICHT_CEILING:
        target->type = schemerlicht_object_type_flonum;
        target->value.fl = ceil(arg1->value.fl);
        continue;
      case SCHEMERLICHT_TRUNCATE:
        target->type = schemerlicht_object_type_flonum;
        target->value.fl = trunc(arg1->value.fl);
        continue;
      case SCHEMERLICHT_ROUND:
        target->type = schemerlicht_object_type_flonum;
        target->value.fl = round(arg1->value.fl);
        continue;
      case SCHEMERLICHT_EXP:
        target->type = schemerlicht_object_type_flonum;
        target->value.fl = exp(arg1->value.fl);
        continue;
      case SCHEMERLICHT_LOG:
        target->type = schemerlicht_object_type_flonum;
        target->value.fl = log(arg1->value.fl);
        continue;
      case SCHEMERLICHT_SIN:
        target->type = schemerlicht_object_type_flonum;
        target->value.fl = sin(arg1->value.fl);
        continue;
      case SCHEMERLICHT_COS:
        target->type = schemerlicht_object_type_flonum;
        target->value.fl = cos(arg1->value.fl);
        continue;
      case SCHEMERLICHT_TAN:
        target->type = schemerlicht_object_type_flonum;
        target->value.fl = tan(arg1->value.fl);
        continue;
      case SCHEMERLICHT_ASIN:
        target->type = schemerlicht_object_type_flonum;
        target->value.fl = asin(arg1->value.fl);
        continue;
      case SCHEMERLICHT_ACOS:
        target->type = schemerlicht_object_type_flonum;
        target->value.fl = acos(arg1->value.fl);
        continue;
      case SCHEMERLICHT_ATAN:
        target->type = schemerlicht_object_type_flonum;
        target->value.fl = atan(arg1->value.fl);
        continue;
      case SCHEMERLICHT_SQRT:
        target->type = schemerlicht_object_type_flonum;
        target->value.fl = sqrt(arg1->value.fl);
        continue;
      default:
        goto slow_lane;        
      }
    }    
    case schemerlicht_object_type_fixnum:
    {
    switch (function_id)
      {
      case SCHEMERLICHT_FLOOR:
        target->type = schemerlicht_object_type_fixnum;
        target->value.fx = arg1->value.fx;
        continue;
      case SCHEMERLICHT_CEILING:
        target->type = schemerlicht_object_type_fixnum;
        target->value.fx = arg1->value.fx;
        continue;
      case SCHEMERLICHT_TRUNCATE:
        target->type = schemerlicht_object_type_fixnum;
        target->value.fx = arg1->value.fx;
        continue;
      case SCHEMERLICHT_ROUND:
        target->type = schemerlicht_object_type_fixnum;
        target->value.fx = arg1->value.fx;
        continue;
      case SCHEMERLICHT_EXP:
        target->type = schemerlicht_object_type_flonum;
        target->value.fl = exp(cast(schemerlicht_flonum, arg1->value.fx));
        continue;
      case SCHEMERLICHT_LOG:
        target->type = schemerlicht_object_type_flonum;
        target->value.fl = log(cast(schemerlicht_flonum, arg1->value.fx));
        continue;
      case SCHEMERLICHT_SIN:
        target->type = schemerlicht_object_type_flonum;
        target->value.fl = sin(cast(schemerlicht_flonum, arg1->value.fx));
        continue;
      case SCHEMERLICHT_COS:
        target->type = schemerlicht_object_type_flonum;
        target->value.fl = cos(cast(schemerlicht_flonum, arg1->value.fx));
        continue;
      case SCHEMERLICHT_TAN:
        target->type = schemerlicht_object_type_flonum;
        target->value.fl = tan(cast(schemerlicht_flonum, arg1->value.fx));
        continue;
      case SCHEMERLICHT_ASIN:
        target->type = schemerlicht_object_type_flonum;
        target->value.fl = asin(cast(schemerlicht_flonum, arg1->value.fx));
        continue;
      case SCHEMERLICHT_ACOS:
        target->type = schemerlicht_object_type_flonum;
        target->value.fl = acos(cast(schemerlicht_flonum, arg1->value.fx));
        continue;
      case SCHEMERLICHT_ATAN:
        target->type = schemerlicht_object_type_flonum;
        target->value.fl = atan(cast(schemerlicht_flonum, arg1->value.fx));
        continue;
      case SCHEMERLICHT_SQRT:
        target->type = schemerlicht_object_type_flonum;
        target->value.fl = sqrt(cast(schemerlicht_flonum, arg1->value.fx));
        continue;
      default:
        goto slow_lane;        
      }
    }
    default:
      goto slow_lane;      
    }
  }
slow_lane: