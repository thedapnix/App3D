#include "Interact.h"

/*Home of all functions tied to drawable interactions*/

void Destroy(DrawableInfo info, std::vector<Drawable>& drawables)
{
    for (int i = 0; i < info.interactsWith.size(); i++) //E.g., rectangle door and sphere doorknob
    {
        for (int j = 0; i < drawables.size(); j++)
        {
            if (info.interactsWith.at(i) == j) //Located
            {
                drawables.at(j).GetDrawableInfo().isActive = false;
                break;
            }
        }
    }
    
    info.interactID = 0; //Remove the interaction once we're done
}

int InteractionBaseFunc(DrawableInfo info, std::vector<Drawable>& drawables)
{
    switch (info.interactID)
    {
    case 1:
        Destroy(info, drawables);
        break;
    case 2:
        //
        break;
    case 3:
        //
        break;
    default:
        std::cout << "Invalid interactionID" << std::endl;
        return -1;
    }

    return 0;
}
